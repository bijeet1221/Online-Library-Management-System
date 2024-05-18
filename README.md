# Online-Library-Management-System


This project implements a Library Management System using C. It includes server and client code to manage books and users efficiently, with features like file locking, password-protected login, socket programming, and concurrent access.

## Features

### Admin
- Add a book
- Delete a book
- Search for a book
- Display all books
- Search member information

### Member
- Borrow a book
- Return a book
- Search for a book
- Display all books

### General
- Password-protected login
- File locking through write locks to prevent data corruption during concurrent access
- Efficient book management
- Concurrent client access via socket programming

## Getting Started

### Prerequisites
- GCC compiler
- Make (optional, for easier compilation)
- Linux-based system (for file locking)

### Compilation

You can compile the server and client code using GCC:

```sh
gcc -o server server.c
gcc -o client client.c
```


### Running the Server

First, run the server:

```sh
./server
```

### Running the Client

Then, run the client:

```sh
./client
```

## Usage

Upon running the client, you will be prompted to log in as either an admin or a member. 

### Admin Login

Use the admin credentials to log in. As an admin, you can:
1. Add a book
2. Delete a book
3. Search for a book
4. Display all books
5. Search for member information

### Member Login

Use member credentials to log in. As a member, you can:
1. Borrow a book
2. Return a book
3. Search for a book
4. Display all books

### Password-protected Login

Both admin and member logins are password-protected. Credentials are stored in a file and are checked during login.

### File Locking

To ensure data integrity during concurrent access, file locking is implemented using write locks.

### Efficient Book Management

Books are managed efficiently, ensuring that all operations are performed quickly and accurately.

## File Structure

- `server.c`: Server-side code
- `client.c`: Client-side code
- `books.txt`: File to store book information
- `members.txt`: File to store member information


## License

This project is licensed under the MIT License.

---
