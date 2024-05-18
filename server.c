#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

#define PASSWORD "getaccess"
#define ADMIN "admin123"
#define MEMBER "member123"
#define MEMBER_PASSWORD "getaccess"
#define MAX_SIZE 100
#define MAX_ITEMS 3


void extractItems(char* line, char items[MAX_ITEMS][MAX_SIZE])
{
    int i = 0;
    char* token = strtok(line, " ");
    
    while (token != NULL && i < MAX_ITEMS) {
        strcpy(items[i], token);
        token = strtok(NULL, " ");
        i++;
    }
}

int PasswordCheck(char name[],char password[])
{
    FILE *pass = fopen("member.txt","r");
    char line[MAX_SIZE];
    char items[MAX_ITEMS][MAX_SIZE];
    while(fgets(line, sizeof(line), pass) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        extractItems(line, items);
        if(strcmp(name,items[1]) == 0 && strcmp(password,items[2]) == 0)
        {
            fclose(pass);
            return 1;
        }
    }
    fclose(pass);
    return 0;
}

int linesPresent()
{
    FILE *book = fopen("books.txt", "r");
    char line[MAX_SIZE];
    char items[MAX_ITEMS][MAX_SIZE];
    int i=0;
    while (fgets(line, sizeof(line), book) != NULL) 
    {
        // Remove newline character from the end of the line if present
        line[strcspn(line, "\n")] = '\0';
        // Extract items from the line
        i++;
    }
    fclose(book);
    return i;
}


int main() 
{
    struct sockaddr_in serv, cli;
    int sd;
    char buf[100];
    int choice;
    int ret;

    // Create socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Server address setup
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = INADDR_ANY;
    serv.sin_port = htons(5000);

    // Bind socket to address
    if (bind(sd, (struct sockaddr *)&serv, sizeof(serv)) == -1) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(sd, 5) == -1) {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }

    while (1) 
    {
        int nsd;
        int len = sizeof(cli);
        nsd = accept(sd, (struct sockaddr *)&cli, &len);
        if(nsd ==  -1)
        {
            perror("accept failed.");
            exit(-1);
        }

        if (fork() == 0) 
        { // Child process
            close(sd); // Close the listening socket in the child process

            // Receive user type from client
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1) 
            {
                perror("read error");
                exit(EXIT_FAILURE);
            }
            buf[ret] = '\0'; // Null-terminate the received string
            printf("User type received from client: %s\n", buf);
            char received_usertype[20];
            strcpy(received_usertype, buf);

            // Receive username from client
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1)
            {
                perror("read error");
                exit(EXIT_FAILURE);
            }
            buf[ret] = '\0'; // Null-terminate the received string
            char received_username[20];
            strcpy(received_username, buf);

            // Receive password from client
            ret = read(nsd, buf, sizeof(buf) - 1);
            if (ret == -1) 
            {
                perror("read error");
                exit(EXIT_FAILURE);
            }
            buf[ret] = '\0'; // Null-terminate the received string
            char received_password[20];
            strcpy(received_password, buf);

            if(strcmp(received_username,ADMIN)==0 && strcmp(received_password,PASSWORD)==0)
            {
                write(nsd, "Access granted", strlen("Access granted"));
                printf("Access by admin123\n");

                while(1)
                {
                    // read choice
                    read(nsd, &choice, sizeof(int));

                    if(choice == 1)
                    {
                        FILE *book = fopen("books.txt", "a");
                        printf("Inside Critical section ADD\n");

                        struct flock lock;
                        lock.l_type = F_WRLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 0;
                        lock.l_len = 0;
                        lock.l_pid = getpid();

                        // set read lock
                        fcntl(fileno(book), F_SETLKW, &lock);

                        // critical section
                        char bookid[10] ,copies[10];
                        char book_name[20];

                        // read book details
                        read(nsd, bookid, sizeof(bookid));
                        read(nsd, book_name, sizeof(book_name));
                        read(nsd, copies, sizeof(copies));

                        // adding book
                        fprintf(book, "%s %s %s\n",bookid, book_name, copies);

                        // Unlock
                        lock.l_type = F_UNLCK;
                        fcntl(fileno(book), F_SETLK, &lock);
                        printf("Unlock succesfull.\n");
                        fclose(book);

                        write(nsd, "success", sizeof("success"));
                    }
                
                    else if (choice == 2)
                    {
                        FILE *book = fopen("books.txt","r");
                        FILE *temp = fopen("temp.txt","w");
                        printf("Inside Critical section DELETE\n");
        
                        struct flock lock;
                        lock.l_type = F_WRLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 0;
                        lock.l_len = 0;
                        lock.l_pid = getpid();

                        // set read lock
                        fcntl(fileno(book), F_SETLKW, &lock);

                        char delete_bookid[10];
                    
                        //printf("%s\n",delete_bookid);
                        read(nsd, delete_bookid, sizeof(delete_bookid));
                        int found = 0;

                        char line[MAX_SIZE];
                        char items[MAX_ITEMS][MAX_SIZE];
                        while (fgets(line, sizeof(line), book) != NULL) 
                        {
                            // Remove newline character from the end of the line if present
                            line[strcspn(line, "\n")] = '\0';
        
                            // Extract items from the line
                            extractItems(line, items);
        
                            // Print the extracted items
                            if(strcmp(delete_bookid,items[0]) != 0 && items[0] != NULL)
                            {   
                                fprintf(temp, "%s %s %s\n", items[0],items[1],items[2]);
                            } 
                            else   found = 1;
                        }
                        

                        lock.l_type = F_UNLCK;
                        fcntl(fileno(book), F_SETLK, &lock);
                        printf("Unlock succesfull.\n");
                        
                        fclose(temp);
                        fclose(book);
                    
                        remove("books.txt");
                        rename("temp.txt","books.txt");

                        write(nsd, &found, sizeof(int));
                    }

                    else if(choice == 3)
                    {
                        FILE *book = fopen("books.txt", "r");
                        printf("Inside Critical section SEARCH\n");
        
                        struct flock lock;
                        lock.l_type = F_WRLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 0;
                        lock.l_len = 0;
                        lock.l_pid = getpid();

                        // set read lock
                        fcntl(fileno(book), F_SETLKW, &lock);

                        char bookid[10], copies[10], seacrhid[10];
                        char book_name[20];

                        read(nsd, seacrhid, sizeof(seacrhid));

                        int found = 0;

                        char line[MAX_SIZE];
                        char items[MAX_ITEMS][MAX_SIZE];
                        while (fgets(line, sizeof(line), book) != NULL) 
                        {
                            // Remove newline character from the end of the line if present
                            line[strcspn(line, "\n")] = '\0';
        
                            // Extract items from the line
                            extractItems(line, items);
        
                            // Print the extracted items
                            if(strcmp(seacrhid,items[0]) == 0)
                            {   
                                found = 1;
                                break;
                            }
                        }
                        
                        lock.l_type = F_UNLCK;
                        fcntl(fileno(book), F_SETLK, &lock);
                        printf("Unlock succesfull.\n");
                        
                        fclose(book);

                        if(found == 1)
                        {
                            write(nsd, &found, sizeof(int));
                            write(nsd, items[0], sizeof(bookid));
                            write(nsd, items[1], sizeof(book_name));
                            write(nsd, items[2], sizeof(copies));
                        }
                        else
                            write(nsd, &found, sizeof(int));
                    }

                    else if(choice == 4)
                    {
                        int len = linesPresent();
                        write(nsd, &len, sizeof(int));

                        FILE *book = fopen("books.txt", "r");

                        char line[MAX_SIZE];
                        char items[MAX_ITEMS][MAX_SIZE];
                        while (fgets(line, sizeof(line), book) != NULL) 
                        {
                            // Remove newline character from the end of the line if present
                            line[strcspn(line, "\n")] = '\0';

                            // Extract items from the line
                            extractItems(line, items);
                            write(nsd,items,sizeof(items));
                        }
                        fclose(book);   
                    }
                    else if(choice == 5)
                    {
                        FILE *member = fopen("member.txt", "r");
                        printf("Inside Critical section SEARCH\n");
        
                        struct flock lock;
                        lock.l_type = F_WRLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 0;
                        lock.l_len = 0;
                        lock.l_pid = getpid();

                        // set read lock
                        fcntl(fileno(member), F_SETLKW, &lock);

                        char memberid[10], pass[20], seacrhid[10];
                        char member_name[20];

                        read(nsd, seacrhid, sizeof(seacrhid));

                        int found = 0;

                        char line[MAX_SIZE];
                        char items[MAX_ITEMS][MAX_SIZE];
                        while (fgets(line, sizeof(line), member) != NULL) 
                        {
                            // Remove newline character from the end of the line if present
                            line[strcspn(line, "\n")] = '\0';
        
                            // Extract items from the line
                            extractItems(line, items);
        
                            // Print the extracted items
                            if(strcmp(seacrhid,items[0]) == 0)
                            {   
                                found = 1;
                                break;
                            }
                        }
                        
                        lock.l_type = F_UNLCK;
                        fcntl(fileno(member), F_SETLK, &lock);
                        printf("Unlock succesfull.\n");
                        
                        fclose(member);

                        if(found == 1)
                        {
                            write(nsd, &found, sizeof(int));
                            write(nsd, items[0], sizeof(memberid));
                            write(nsd, items[1], sizeof(member_name));
                            write(nsd, items[2], sizeof(pass));
                        }
                        else
                            write(nsd, &found, sizeof(int));
                    }
                }
            }
            else if(PasswordCheck(received_username,received_password) == 1)
            {
                  
                write(nsd, "Access granted", strlen("Access granted"));
                printf("Access by %s\n",received_username);

                while(1)
                {
                    read(nsd, &choice, sizeof(int));

                    if(choice == 1)
                    {
                        FILE *book = fopen("books.txt","r+");
                        FILE *temp = fopen("temp.txt","w");
                        printf("Inside Critical section BORROW\n");
        
                        struct flock lock;
                        lock.l_type = F_WRLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 0;
                        lock.l_len = 0;
                        lock.l_pid = getpid();
    
                        // set write lock
                        int fd = fileno(book);
                        if (fd == -1)  
                        {
                            perror("fileno error");
                            fclose(book);
                            fclose(temp);
                            return 0; // Return 0 to indicate failure
                        }
                        if (fcntl(fd, F_SETLKW, &lock) == -1) 
                        {
                            perror("fcntl");
                            fclose(book);
                            fclose(temp);
                            return 0; // Return 0 to indicate failure
                        }
                        printf("Locked\n");

                        char borrow_bookid[10];
                    
                        //printf("%s\n",delete_bookid);
                        read(nsd, borrow_bookid, sizeof(borrow_bookid));
                        int found = 0;

                        char line[MAX_SIZE];
                        char items[MAX_ITEMS][MAX_SIZE];
                        while (fgets(line, sizeof(line), book) != NULL) 
                        {
                            // Remove newline character from the end of the line if present
                            line[strcspn(line, "\n")] = '\0';
        
                            // Extract items from the line
                            extractItems(line, items);
        
                            // Print the extracted items
                            if(strcmp(borrow_bookid,items[0]) != 0 && items[0] != NULL)
                            {   
                                fprintf(temp, "%s %s %s\n", items[0],items[1],items[2]);
                            } 
                            else
                            {
                                found = 1;
                                int new_copies = atoi(items[2]);
                                new_copies--;
                                sprintf(items[2], "%d",new_copies);
                                fprintf(temp,"%s %s %s\n", items[0],items[1],items[2]);
                            }
                        }
                        

                        lock.l_type = F_UNLCK;
                        if(fcntl(fd, F_SETLK, &lock) == -1)
                        {
                            perror("fcntl - unlocking");
                        }

                        printf("Unlock succesfull.\n");
                        
                        fclose(temp);
                        fclose(book);
                    
                        remove("books.txt");
                        rename("temp.txt","books.txt");

                        write(nsd, &found, sizeof(int));
                    }

                    else if(choice == 2)
                    {
                        FILE *book = fopen("books.txt","r+");
                        FILE *temp = fopen("temp.txt","w");
                        printf("Inside Critical section RETURN\n");
        
                        struct flock lock;
                        lock.l_type = F_WRLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 0;
                        lock.l_len = 0;
                        lock.l_pid = getpid();

                        // set write lock
                        int fd = fileno(book);
                        if (fd == -1)  
                        {
                            perror("fileno error");
                            fclose(book);
                            fclose(temp);
                            return 0; // Return 0 to indicate failure
                        }
                        if (fcntl(fd, F_SETLKW, &lock) == -1) 
                        {
                            perror("fcntl");
                            fclose(book);
                            fclose(temp);
                            return 0; // Return 0 to indicate failure
                        }
                        printf("Locked\n");

                        char borrow_bookid[10];
                    
                        //printf("%s\n",delete_bookid);
                        read(nsd, borrow_bookid, sizeof(borrow_bookid));
                        int found = 0;

                        char line[MAX_SIZE];
                        char items[MAX_ITEMS][MAX_SIZE];
                        while (fgets(line, sizeof(line), book) != NULL) 
                        {
                            // Remove newline character from the end of the line if present
                            line[strcspn(line, "\n")] = '\0';
        
                            // Extract items from the line
                            extractItems(line, items);
        
                            // Print the extracted items
                            if(strcmp(borrow_bookid,items[0]) != 0 && items[0] != NULL)
                            {   
                                fprintf(temp, "%s %s %s\n", items[0],items[1],items[2]);
                            } 
                            else
                            {
                                found = 1;
                                int new_copies = atoi(items[2]);
                                new_copies++;
                                sprintf(items[2], "%d",new_copies);
                                fprintf(temp,"%s %s %s\n", items[0],items[1],items[2]);
                            }
                        }
                        

                        lock.l_type = F_UNLCK;
                        if(fcntl(fd, F_SETLK, &lock) == -1)
                        {
                            perror("fcntl - unlocking");
                        }

                        printf("Unlock succesfull.\n");
                        
                        fclose(temp);
                        fclose(book);
                    
                        remove("books.txt");
                        rename("temp.txt","books.txt");

                        write(nsd, &found, sizeof(int));
                    }

                    else if(choice == 3)
                    {
                        FILE *book = fopen("books.txt", "r");
                        printf("Inside Critical section SEARCH\n");
        
                        struct flock lock;
                        lock.l_type = F_RDLCK;
                        lock.l_whence = SEEK_SET;
                        lock.l_start = 0;
                        lock.l_len = 0;
                        lock.l_pid = getpid();

                        // set read lock
                        fcntl(fileno(book), F_SETLKW, &lock);

                        char bookid[10], copies[10], searchid[10];
                        char book_name[20];

                        read(nsd, searchid, sizeof(searchid));

                        int found = 0;

                        char line[MAX_SIZE];
                        char items[MAX_ITEMS][MAX_SIZE];
                        while (fgets(line, sizeof(line), book) != NULL) 
                        {
                            // Remove newline character from the end of the line if present
                            line[strcspn(line, "\n")] = '\0';
        
                            // Extract items from the line
                            extractItems(line, items);
        
                            // Print the extracted items
                            if(strcmp(searchid,items[0]) == 0)
                            {   
                                found = 1;
                                break;
                            }
                        }
                        
                        lock.l_type = F_UNLCK;
                        fcntl(fileno(book), F_SETLK, &lock);
                        printf("Unlock succesfull.\n");
                        
                        fclose(book);

                        if(found == 1)
                        {
                            write(nsd, &found, sizeof(int));
                            write(nsd, items[0], sizeof(bookid));
                            write(nsd, items[1], sizeof(book_name));
                            write(nsd, items[2], sizeof(copies));
                        }
                        else
                            write(nsd, &found, sizeof(int));
                    }
                    
                    else if(choice == 4)
                    {
                        int len = linesPresent();
                        write(nsd, &len, sizeof(int));

                        FILE *book = fopen("books.txt", "r");

                        char line[MAX_SIZE];
                        char items[MAX_ITEMS][MAX_SIZE];
                        while (fgets(line, sizeof(line), book) != NULL) 
                        {
                            // Remove newline character from the end of the line if present
                            line[strcspn(line, "\n")] = '\0';

                            // Extract items from the line
                            extractItems(line, items);
                            write(nsd,items,sizeof(items));
                        }
                        fclose(book);
                    }
                }
            }
            else
            {
                 //write(nsd, "Access denied.", sizeof("Access denied."));
                 exit(-1);
            }
            close(nsd);
            exit(0);
        }
        close(nsd);
    }
    
}