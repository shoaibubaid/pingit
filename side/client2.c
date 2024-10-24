#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8082
#define MAX_BUFFER_SIZE 1024

int sockfd;

// Thread function to handle sending messages to the server
void* send_message(void* arg) {
    char buffer[MAX_BUFFER_SIZE];
    while (1) {
        fgets(buffer, MAX_BUFFER_SIZE, stdin);  // Get message from client's stdin
        write(sockfd, buffer, strlen(buffer));  // Send message to server
        if (strncmp(buffer, "exit", 4) == 0) {  // Exit if "exit" is typed
            close(sockfd);
            exit(0);
        }
    }
    return NULL;
}

// Thread function to handle receiving messages from the server
void* receive_message(void* arg) {
    char buffer[MAX_BUFFER_SIZE];
    int ret;
    while (1) {
        memset(buffer, 0, MAX_BUFFER_SIZE);
        ret = read(sockfd, buffer, MAX_BUFFER_SIZE - 1);  // Read message from server
        if (ret <= 0) {
            perror("Read from server failed");
            break;
        }
        buffer[ret] = '\0';  // Null-terminate the received string
        printf("Server: %s", buffer);  // Print the received message
        if (strncmp(buffer, "exit", 4) == 0) {
            close(sockfd);
            exit(0);
        }
    }
    return NULL;
}

int main() {
    struct sockaddr_in server_addr;
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
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        close(sockfd);
        return -1;
    }

    printf("Connected to the server.\n");

    // Create threads for sending and receiving messages
    pthread_create(&send_thread, NULL, send_message, NULL);
    pthread_create(&receive_thread, NULL, receive_message, NULL);

    // Wait for threads to finish
    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    return 0;
}
