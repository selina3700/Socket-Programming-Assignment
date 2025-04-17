#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h> // Windows-specific socket headers
#include <pthread.h>  // POSIX threading library for Windows
#include <ws2tcpip.h>

#pragma comment(lib, "ws2_32.lib") // Link Winsock library

/*Line 14 to line 139 was modified and extracted from How do I make a simple chat room using C 
programming language? I know the basics of TCP, UDP, domain name system, etc. 
from YouTube and I... (n.d.). Quora. 
https://www.quora.com/How-do-I-make-a-simple-chat-room-using-C-programming-language-I-know-the-basics-of-TCP-UDP-domain-name-system-etc-from-YouTube-and-I-can-program-in-C-I-have-2-weeks-to-do-this-at-most‌*/
static void init_winsock() __attribute__((constructor));
static void cleanup_winsock() __attribute__((destructor));

// Added for Windows
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

#define PORT 8080
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define close(fd) closesocket(fd)

int clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
    int client_socket = *(int *)arg;
    char buffer[BUFFER_SIZE];
    int read_size;

    while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        buffer[read_size] = '\0'; // Null-terminate the string
        printf("Client: %s\n", buffer);

        // Broadcast the message to all clients
        pthread_mutex_lock(&clients_mutex);
        for (int i = 0; i < client_count; i++) {
            if (clients[i] != client_socket) {
                if (send(clients[i], buffer, read_size, 0) == SOCKET_ERROR) {
                    printf("Failed to send message to a client. Error Code: %d\n", WSAGetLastError());
                }
            }
        }
        pthread_mutex_unlock(&clients_mutex);
    }

    if (read_size == SOCKET_ERROR) {
        printf("Receive failed. Error Code: %d\n", WSAGetLastError());
    }

    // Remove client from clients array
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (clients[i] == client_socket) {
            clients[i] = clients[client_count - 1];
            client_count--;
            break;
        }
    }
    pthread_mutex_unlock(&clients_mutex);

    closesocket(client_socket); // Use closesocket on Windows
    return NULL;
}

int main() {
   
    SOCKET server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    int addr_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        fprintf(stderr, "WSAStartup failed\n");
        return -1;
    }

    // Setup server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed. Error Code: %d\n");
        closesocket(server_socket);
        return -1; 
    }

    // Listen for incoming connections
    listen(server_socket, 3);
    printf("Chat server started on port %d\n", PORT);

    while (1) {
        // Accept a new client
        client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len);
        if (client_socket == INVALID_SOCKET) {
            printf("Accept failed. Error Code: %d\n");
            continue;
        }

        // Add client to clients array
        pthread_mutex_lock(&clients_mutex);
        if (client_count < MAX_CLIENTS) {
            clients[client_count++] = client_socket;
        } else {
            printf("Max clients reached. Connection refused.\n");
            closesocket(client_socket);
        }
        pthread_mutex_unlock(&clients_mutex);

        // Create a new thread for the client
        pthread_t tid;
        if (pthread_create(&tid, NULL, handle_client, (void *)&client_socket) != 0) {
            printf("Failed to create thread\n");
            closesocket(client_socket);
            continue;
        }
        pthread_detach(tid);
    }

    closesocket(server_socket);
    
    return 0;
}