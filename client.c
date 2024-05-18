#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/types.h>

#define MAX_USER_TYPE_LENGTH 20
#define MAX_USERNAME_LENGTH 20
#define MAX_PASSWORD_LENGTH 20
#define MAX_MEMBER_USERNAME_LENGTH 20
#define MAX_SIZE 100
#define MAX_ITEMS 3

int main()
{
    struct sockaddr_in serv;
    int sd;
    char user_type[MAX_USER_TYPE_LENGTH];
    char entered_username[MAX_USERNAME_LENGTH];
    char entered_password[MAX_PASSWORD_LENGTH];
    char member_username[MAX_MEMBER_USERNAME_LENGTH];
    int ret;
    int choice;

    // Create socket
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd == -1)
    {
        perror("socket creation failed");
        exit(-1);
    }
    
    // Server address setup
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr("127.0.0.1"); // Change this to your server IP
    serv.sin_port = htons(5000); // Change this to your server port

    // Connect to the server
    if (connect(sd, (struct sockaddr *)&serv, sizeof(serv)) == -1) 
    {
        perror("connection failed");
        exit(-1);
    }
    
    // Authentication
    printf("Welcome to the online library\n");

    printf("Are you an admin or a member? (admin/member): ");
    scanf("%9s", user_type);
    
    if (strcmp(user_type, "admin") == 0)
    {
        write(sd, user_type, strlen(user_type));
    
        printf("Enter your username: ");
        scanf("%19s", entered_username);
        write(sd, entered_username, strlen(entered_username));
        
        printf("Enter your password: ");
        scanf("%19s", entered_password);
        write(sd, entered_password, strlen(entered_password));
    
        // Read response from server
        char response[50];
        read(sd, response, sizeof(response));
        
        if(strcmp(response,"Access granted") == 0)
        {
            printf("Access granted\n");
            while(1)
            {
                printf("What would like to do?\n");
                printf("1. Add a book\n");
                printf("2. Remove a book\n");
                printf("3. Search for a book\n");
                printf("4. Display all books\n");
                printf("5. Member info\n");
                printf("6. Exit\n");
                printf("\n");
                printf("Enter choice :\n");
                scanf("%d", &choice);
    
                write(sd, &choice, sizeof(int));
    
                if(choice == 1)
                {
                    char bookid[10],copies[10],flag[10];
                    char book_name[20];
                    printf("Enter book id\n");
                    scanf("%s", bookid );
                    write(sd, bookid, sizeof(int));
    
                    printf("Enter book name\n");
                    scanf("%s" ,book_name );
                    write(sd, book_name, sizeof(book_name));
    
                    printf("Enter books copies\n");
                    scanf( "%s" , copies );
                    write(sd, copies, sizeof(copies));
    
                    read(sd, flag,sizeof(flag));
    
                    if(strcmp(flag,"success") == 0)
                        printf("Book added successfully.\n");
                }
    
                else if(choice == 2)
                {
                    char bookid[10];
                    int found = 0;
                    printf("Enter bookid :\n");
                    scanf("%s", bookid);
                    write(sd, bookid, sizeof(bookid));
    
                    read(sd, &found, sizeof(int));
    
                    if(found == 1)
                        printf("Book deleted successfully.\n");
                    else
                        printf("book not found.\n");
                }
                else if(choice == 3)
                {
                    char bookid[10],book_name[20],copies[10];
                    int response = 0;
                    printf("Enter bookid:\n");
                    scanf("%s", bookid);
                    write(sd, bookid, sizeof(bookid));
    
                    read(sd, &response, sizeof(int));
    
                    if(response == 1)
                    {
                        //printf("%s\n",response);
                        read(sd, bookid, sizeof(bookid));
                        read(sd, book_name, sizeof(book_name));
                        read(sd, copies, sizeof(copies));

                        printf("Book found: \n");
                        printf("Book id :%s\n",bookid);
                        printf("Book name :%s\n",book_name);
                        printf("Copies available :%s\n",copies);
                    }
                    else
                        printf("Not found.\n");
                }

                else if(choice == 4)
                {
                    int len;
                    read(sd, &len, sizeof(int));
                    char items[MAX_ITEMS][MAX_SIZE];
                    
                    for(int i=0;i<len;i++)
                    {
                        read(sd,items,sizeof(items));
                        printf("book id = %s\n",items[0]);
                        printf("book name = %s\n",items[1]);
                        printf("copies = %s\n",items[2]);
                    }
                }
    
                else if(choice == 5)
                {
                    char memberid[10],member_name[20],pass[20];
                    int response = 0;
                    printf("Enter memberid:\n");
                    scanf("%s", memberid);
                    write(sd, memberid, sizeof(memberid));
    
                    read(sd, &response, sizeof(int));
    
                    if(response == 1)
                    {
                        //printf("%s\n",response);
                        read(sd, memberid, sizeof(memberid));
                        read(sd, member_name, sizeof(member_name));
                        read(sd, pass, sizeof(pass));

                        printf("Member found: \n");
                        printf("Member id :%s\n",memberid);
                        printf("Member name :%s\n",member_name);
                        printf("Password :%s\n",pass);
                    }
                    else
                        printf("Not found.\n");
                }
                
                else if(choice == 6)
                    exit(1);
            }
        }
    }
    else if(strcmp(user_type,"member") == 0)
    {
        write(sd, user_type, strlen(user_type));
    
        printf("Enter your username: ");
        scanf("%19s", entered_username);
        write(sd, entered_username, strlen(entered_username));
        
        printf("Enter your password: ");
        scanf("%19s", entered_password);
        write(sd, entered_password, strlen(entered_password));
    
        // Read response from server
        char response[50];
        read(sd, response, sizeof(response));
        
        if(strcmp(response,"Access granted") == 0)
        {
            printf("Access granted.\n");
            while(1)
            {
                printf("What would like to do?\n");
                printf("1. Borrow book\n");
                printf("2. Return a book\n");
                printf("3. Search for a book\n");
                printf("4. Display all books\n");
                printf("5. Exit\n");
                printf("\n");
                printf("Enter choice\n");
                scanf("%d", &choice);

                write(sd, &choice, sizeof(int));


                if(choice == 1)
                {
                    char bookid[10];
                    int response = 0;
                    printf("Enter book id :\n");
                    scanf("%s",bookid);
                    write(sd, bookid, sizeof(bookid));

                    read(sd, &response, sizeof(int));
                    if(response == 1)
                        printf("Borrowed\n");
                    else
                        printf("Failed\n");

                }
                
                if(choice == 2)
                {
                    char bookid[10];
                    int response = 0;
                    printf("Enter book id :\n");
                    scanf("%s",bookid);
                    write(sd, bookid, sizeof(bookid));

                    read(sd, &response, sizeof(int));
                    if(response == 1)
                        printf("Returned\n");
                    else
                        printf("Failed\n");

                }

                else if(choice == 3)
                {
                    char bookid[10],book_name[20],copies[10];
                    int response = 0;
                    printf("Enter bookid:\n");
                    scanf("%s", bookid);
                    write(sd, bookid, sizeof(bookid));
    
                    read(sd, &response, sizeof(int));
    
                    if(response == 1)
                    {
                        //printf("%s\n",response);
                        read(sd, bookid, sizeof(bookid));
                        read(sd, book_name, sizeof(book_name));
                        read(sd, copies, sizeof(copies));

                        printf("Book found: \n");
                        printf("Book id :%s\n",bookid);
                        printf("Book name :%s\n",book_name);
                        printf("Copies available :%s\n",copies);
                    }
                    else
                        printf("Not found.\n");
                }
                
                
                else if(choice == 4)
                {
                    int len;
                    read(sd, &len, sizeof(int));
                    char items[MAX_ITEMS][MAX_SIZE];
                    
                    for(int i=0;i<len;i++)
                    {
                        read(sd,items,sizeof(items));
                        printf("book id = %s\n",items[0]);
                        printf("book name = %s\n",items[1]);
                        printf("copies = %s\n",items[2]);
                    }
                }

                else if(choice == 5)
                    exit(1);
            }
        }
    }
    else
    {
        exit(-1);
    }
}
