#include <stdio.h>          // For standard input/output functions (printf, fgets, etc.)
#include <stdlib.h>         // For general utilities (exit, etc.)
#include <string.h>         // For memory and string functions (memset, strlen, etc.)
#include <unistd.h>         // For close(), read(), write(), etc.
#include <arpa/inet.h>      // For socket programming functions and structures (sockaddr_in, inet_pton, etc.)

#define PORT 9090           // The port number that the client will connect to
#define BUFFER_SIZE 50    // The size of the message buffer for sending/receiving data

int main() {
    int sock = 0;                               // Socket file descriptor
    struct sockaddr_in serv_addr;               // Structure to hold server address information
    char buffer[BUFFER_SIZE] = {0};             // Buffer for storing data to send or receive

    // Create socket (IPv4, TCP)
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");  // Error message if socket creation fails
        return -1;                              // Return -1 to indicate failure
    }

    serv_addr.sin_family = AF_INET;             // Use IPv4 address family
    serv_addr.sin_port = htons(PORT);           // Convert the port number to network byte order

    // Convert textual IP address ("127.0.0.1") to binary form and store in serv_addr.sin_addr
    // inet_pton() returns 1 if successful, 0 if invalid format, -1 if error
    if (inet_pton(AF_INET, "192.168.1.114", &serv_addr.sin_addr) <= 0) {                          // ZMIANA IP !!!!!
        printf("\nInvalid address/ Address not supported \n"); // Error if IP address is invalid
        return -1;                              // Return -1 to indicate failure
    }

    // Connect the socket to the specified server address
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");       // Error message if connection attempt fails
        return -1;                              // Return -1 to indicate failure
    }

    printf("Connected to server!\n");

    char name[BUFFER_SIZE] = {0};
    printf("Enter your name: ");
    fgets(name, BUFFER_SIZE, stdin);
    name[strcspn(name, "\n")] = 0;
    send(sock, name, strlen(name), 0);

    // Continuous communication loop
    while (1) {
        printf("\033[1;32mClient:\033[0m ");             // Prompt user for input
        fgets(buffer, BUFFER_SIZE, stdin);      // Read message from keyboard into buffer
        if (buffer[0] == '\n' || buffer[0] == '\0') {   // nowy warunek
            printf("Cannot send empty message.\n");     
            continue;                                  
        }
        send(sock, buffer, strlen(buffer), 0);

        memset(buffer, 0, BUFFER_SIZE);         // Clear buffer before reading the server’s reply
        int valread = read(sock, buffer, BUFFER_SIZE); // Read message from server
        if (valread <= 0) {                     // If no data received or server closed connection
            printf("Server closed the connection.\n");
            break;                              // Exit communication loop
        }
        printf("\033[1;34mServer: %s\033[0m\n", buffer);         // Display message received from server
    }

    close(sock);                                // Close the socket after communication ends
    return 0;                                   // Normal program termination
}
