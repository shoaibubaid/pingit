#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8082
#define MAX_BUFFER_SIZE 1024
#define BACKLOG 10

int sockfd, newfd;

// Thread function to handle sending messages to the client
void* send_message(void* arg) {
    char buffer[MAX_BUFFER_SIZE];
    while (1) {
        fgets(buffer, MAX_BUFFER_SIZE, stdin);  // Get message from server's stdin
        write(newfd, buffer, strlen(buffer));   // Send message to the client
        if (strncmp(buffer, "exit", 4) == 0) {  // Exit if "exit" is typed
            close(newfd);
            close(sockfd);
            exit(0);
        }
    }
    return NULL;
}

// Thread function to handle receiving messages from the client
void* receive_message(void* arg) {
    char buffer[MAX_BUFFER_SIZE];
    int ret;
    while (1) {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        ret = read(newfd, buffer, MAX_BUFFER_SIZE - 1);  // Read message from client
        if (ret <= 0) {
            perror("Read from client failed");
            break;
        }
        buffer[ret] = '\0';  // Null-terminate the received string
        printf("Client: %s", buffer);  // Print the received message
        if (strncmp(buffer, "exit", 4) == 0) {
            close(newfd);
            close(sockfd);
            exit(0);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len;
    pthread_t send_thread, receive_thread;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        return -1;
    }

    // Set server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    // Listen for connections
    if (listen(sockfd, BACKLOG) < 0) {
        perror("Listen failed");
        return -1;
    }

    printf("Server listening on port %d...\n", PORT);

    // Accept client connection
    addr_len = sizeof(client_addr);
    newfd = accept(sockfd, (struct sockaddr*)&client_addr, &addr_len);
    if (newfd < 0) {
        perror("Connection accept failed");
        return -1;
    }

    printf("Client connected.\n");

    // Create threads for sending and receiving messages
    pthread_create(&send_thread, NULL, send_message, NULL);
    pthread_create(&receive_thread, NULL, receive_message, NULL);

    // Wait for threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    return 0;
}
