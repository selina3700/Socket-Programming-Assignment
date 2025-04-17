#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // Windows-specific socket headers
#include <pthread.h>  // POSIX threading library for Windows
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library
/*Line 14 to line 85 was modified and extracted from How do I make a simple chat room using C 
programming language? I know the basics of TCP, UDP, domain name system, etc. 
from YouTube and I... (n.d.). Quora. 
https://www.quora.com/How-do-I-make-a-simple-chat-room-using-C-programming-language-I-know-the-basics-of-TCP-UDP-domain-name-system-etc-from-YouTube-and-I-can-program-in-C-I-have-2-weeks-to-do-this-at-most‌*/

#define PORT 8080
#define BUFFER_SIZE 1024

void *receive_messages(void *socket) {
    int sock = *(int *)socket;
    char buffer[BUFFER_SIZE];

    while (1) {
        int read_size = recv(sock, buffer, sizeof(buffer), 0);
        if (read_size > 0) {
            buffer[read_size] = '\0'; // Null-terminate the string
            printf("Message: %s\n", buffer);
        } else {
            printf("Server disconnected\n");
            break;
        }
    }
    return NULL;
}

int main() {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("Failed to initialize Winsock. Error Code: %d\n", WSAGetLastError());
        exit(EXIT_FAILURE);
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) {
        printf("Socket creation failed. Error Code: %d\n", WSAGetLastError());
        return -1;
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to server
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Connection failed. Error Code: %d\n", WSAGetLastError());
        return -1;
    }

    // Create a thread to receive messages
    pthread_t tid;
    if (pthread_create(&tid, NULL, receive_messages, (void *)&sock) != 0) {
        printf("Failed to create thread\n");
        closesocket(sock);
        return -1;
    }
    pthread_detach(tid);

    // Main loop for sending messages
    while (1) {
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
        if (send(sock, buffer, strlen(buffer), 0) == SOCKET_ERROR) {
            printf("Send failed. Error Code: %d\n", WSAGetLastError());
        }
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
