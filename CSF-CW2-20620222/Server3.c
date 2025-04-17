#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>  
#include <ws2tcpip.h>  
#include <ctype.h> 
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")  

#define close(fd) closesocket(fd) 
#define SIZE 50 
// Automatic Winsock initialization and cleanup
static void init_winsock() __attribute__((constructor));
static void cleanup_winsock() __attribute__((destructor));

static void init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }
}

static void cleanup_winsock() {
    WSACleanup();
}

#define PORT 8080  // Port number for the server

int server_fd, new_socket;
struct sockaddr_in address;
int addrlen = sizeof(address);
char buffer[1024] = {0};
char revbuffer[1024];
int i, j, length;
char *message = buffer;

int main() {
    time_t t ;
    struct tm *tmp ;
    char current_time[SIZE];
    time(&t);

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to an IP/Port
    address.sin_family = AF_INET;          // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Any IP address
    address.sin_port = htons(PORT);        // Host to Network Short

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    while(1){
        // 4. Accept a connection
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (new_socket == -1) {
            perror("Accepting connection failed");
            close(server_fd);
            exit(EXIT_FAILURE);
        }

        // 5. Receive data from the client
            recv(new_socket, buffer, sizeof(buffer), 0);
            printf("Received from client: %s\n", buffer);

        // 7. Get IP address and Port of Client
        /*Line 87 to line 89 was extracted and modified from Beej’s Guide to Network Programming. (2024).        Beej.us. https://beej.us/guide/bgnet/html/#inet_ntopman‌*/
        struct sockaddr_in sa;
        char str[INET_ADDRSTRLEN]; //Declare a character array to store the IP address in string format
        inet_ntop(AF_INET, &(address.sin_addr), str, INET_ADDRSTRLEN);        printf("IP address: %s\n",str); //Prints the IP address 

        /*Line 93 was extracted and modified from REALFREE. (2024). How to get ip address from sock structure in c? 
        Stack Overflow. https://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c*/
        printf("Port Number: %d\n", (int) ntohs(address.sin_port)); //Prints the Port Number
        
        //Exit server if client enters "Exit Server"
        if (strcmp(buffer, "Exit Server") == 0) {
            const char *exit_msg = "Exit command received. Shutting down server.\n";
            printf("Exit command received. Shutting down server.\n");
            send(new_socket, exit_msg, strlen(exit_msg), 0);
            break;

        //Display date and time when the command "Date" is entered
        /*Line 106 to line 115 was extracted and modified from strftime() function in C/C++. (2017, April 4). 
        GeeksforGeeks. https://www.geeksforgeeks.org/strftime-function-in-c/ and also strftime. (2024). 
        Opengroup.org. https://pubs.opengroup.org/onlinepubs/009696799/functions/strftime.html*/
        } else if (strcmp(buffer, "Date") == 0) {
            tmp = localtime(&t);
            strftime(current_time, sizeof(current_time), "%a %b %d %T %Y GMT %z", tmp);
            printf("Date & Time: %s\r\n",current_time);
            send(new_socket, current_time, strlen(current_time), 0);
        } else {
            const char *invalid_msg = "ERROR! Valid commands are: 'Exit Server', 'Date'.\n";
            send(new_socket, invalid_msg, strlen(invalid_msg), 0);
            printf("Error response sent to client.\n");
        }

        // 8. Send response to the client
        // Send the modified buffer back to the client
        send(new_socket, buffer, strlen(buffer), 0);
        printf("Sent to client: %s\n", buffer);

        close(new_socket); // Close connection with the client
    }
    // 9. Close the sockets
    close(new_socket);
    close(server_fd);

    return 0;
}