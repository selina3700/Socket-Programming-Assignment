#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

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

int main() {
    init_winsock();

    int client_fd;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    const char *message = buffer;

    // Create the socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == INVALID_SOCKET) {
        fprintf(stderr, "Socket creation failed: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "Connection failed: %d\n", WSAGetLastError());
        closesocket(client_fd);
        cleanup_winsock();
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server.\n");

    // Send data to the server
    printf("Enter Message: ");
    fgets(buffer, sizeof(buffer), stdin);
    buffer[strcspn(buffer, "\n")] = '\0'; // Remove trailing newline

    if (send(client_fd, message, strlen(message), 0) < 0) {
        printf("Send failed: %d\n", WSAGetLastError());
    }
    printf("Sent to server: %s\n", message);

    // Check for "Exit Server"
    if (strcasecmp(buffer, "Exit Server") == 0) {
        printf("Exiting client.\n");
    }

    // Receive response from the server
    memset(buffer, 0, sizeof(buffer));
    int bytes_received = recv(client_fd, buffer, sizeof(buffer), 0);
        
    if (bytes_received <= 0) {
        if (bytes_received == 0) {
            printf("Server closed the connection.\n");
        } else {
            printf("Receive failed: %d\n", WSAGetLastError());
        }
    }
        
    buffer[bytes_received] = '\0';
    printf("Received from server: %s\n", buffer);
    memset(buffer, 0, sizeof(buffer));

    // Close the socket
    closesocket(client_fd);
    cleanup_winsock();
    return 0;
}