#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>  
#include <ws2tcpip.h>  
#include <ctype.h> 

void reverse(char *buffer);
void upper(char *buffer);
void stringlength(char *buffer);
int alphanumeric(const char *buffer);

#pragma comment(lib, "Ws2_32.lib")  

#define close(fd) closesocket(fd) 

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
            continue;
        }

        // 5. Receive data from the client
            recv(new_socket, buffer, sizeof(buffer), 0);
            printf("Received from client: %s\n", buffer);
            stringlength (buffer);      //Get string length

        // 7. Get IP address and Port of Client
        /*Line 85 to line 88 was extracted and modified from Beej’s Guide to Network Programming. (2024).        Beej.us. https://beej.us/guide/bgnet/html/#inet_ntopman‌*/
        struct sockaddr_in sa;
        char str[INET_ADDRSTRLEN]; //Declare a character array to store the IP address in string format
        inet_ntop(AF_INET, &(address.sin_addr), str, INET_ADDRSTRLEN);       
        printf("IP address: %s\n",str); //Prints the IP address 

        /*Line 92 was extracted and modified from REALFREE. (2024). How to get ip address from sock structure in c? 
        Stack Overflow. https://stackoverflow.com/questions/3060950/how-to-get-ip-address-from-sock-structure-in-c*/
        printf("Port Number: %d\n", (int) ntohs(address.sin_port)); //Prints the Port Number
        
        //Exit server if client enters "Exit Server"
        if (strcmp(buffer, "Exit Server") == 0) {
            const char *exit_msg = "Exit command received. Shutting down server.\n";
            printf("Exit command received. Shutting down server.\n");
            send(new_socket, exit_msg, strlen(exit_msg), 0);
            break;
        }

        // 8. Send response to the client
        if (!alphanumeric(buffer)) {
            const char *invalid_msg = "ERROR! Message contains non-alphanumeric characters.";
            send(new_socket, invalid_msg, strlen(invalid_msg), 0);
            printf("Error response sent to client.\n");
        } else {
            //Reverse the message
            reverse (buffer);
            //Uppercase client input
            upper(buffer);
            // Send the modified buffer back to the client
            send(new_socket, buffer, strlen(buffer), 0);
            printf("Sent to client: %s\n", buffer);
        }
    }
    // 9. Close the sockets
    close(new_socket);
    close(server_fd);

    return 0;
}

//Function to uppercase client input
/*Line 127 to 132 was extracted and modified from Convert a String to Uppercase in C. (n.d.). 
Www.tutorialspoint.com. https://www.tutorialspoint.com/convert-a-string-to-uppercase-in-c*/
void upper(char *buffer){
    for (i = 0; buffer[i]!='\0'; i++) {  
        if(buffer[i] >= 'a' && buffer[i] <= 'z') {
            buffer[i] = buffer[i] -32; //Convert into uppercase by substracting 32 from their ASCII value
        }
    }
}

//Function to reverse client input
/*Line 138 to line 152 was modified and extracted from GeeksforGeeks. (2023, November 28). 
Reverse String in C. GeeksforGeeks. https://www.geeksforgeeks.org/reverse-string-in-c/‌*/
void reverse(char* buffer){
    //Initialize pointers
    int firstChar = 0;
    int lastChar = strlen(buffer) - 1;
    char temp;
    while (firstChar < lastChar) {
        //swap characters
        temp = buffer[firstChar];
        buffer[firstChar] = buffer[lastChar];
        buffer[lastChar] = temp;
        //moves the pointers towards each other
        firstChar++;
        lastChar--;
    }
}

//Function to get string length of user input
/*Line 157 to 160 was modifies and extracted from C program to find the length of a string. (2018, October 8).
GeeksforGeeks. https://www.geeksforgeeks.org/c-program-to-find-the-length-of-a-string/‌*/
void stringlength(char *buffer){
    size_t len = strlen(buffer); // Use size_t for string length
    printf("Length of string: %lu\n", len);
}

//Function to check for alphanumeric characters in string
/*Line 165 to line 172 was extracted and modified from isalnum(). (2024, September 27). Vultr.com; Vultr. 
https://docs.vultr.com/clang/standard-library/ctype-h/isalnum‌*/
int alphanumeric(const char *message){
    for (int i = 0; i < strlen(message); i++) {
        if (!isalnum(message[i])) {
            return 0; //Input is not alphanumeric
        }
    }
    return 1; //Input is alphanumeric
}