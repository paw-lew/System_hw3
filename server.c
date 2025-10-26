#include <stdio.h>          // For input/output functions (printf, perror, etc.)
#include <stdlib.h>         // For exit() and general utilities
#include <string.h>         // For string handling (memset, strlen, etc.)
#include <unistd.h>         // For close(), read(), write(), etc.
#include <arpa/inet.h>      // For internet address structures and functions (socket, bind, etc.)

#define PORT 9090           // Define the port number on which the server will listen
#define BUFFER_SIZE 50    // Define the size of the buffer for message exchange

int main() {
    int server_fd, new_socket;                  // server_fd = server socket descriptor, new_socket = socket for accepted client
    struct sockaddr_in address;                 // Structure to store address information
    int opt = 1;                                // Option value for setsockopt (enabling SO_REUSEADDR)
    int addrlen = sizeof(address);              // Length of the address structure
    char buffer[BUFFER_SIZE] = {0};             // Buffer to store incoming and outgoing messages, initialized to zero

    // Create socket file descriptor (IPv4, TCP, protocol default)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");                // Print error if socket creation fails
        exit(EXIT_FAILURE);                     // Exit program if socket creation fails
    }

    // Set socket options to allow reuse of local addresses (helps avoid “address already in use” errors)
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");                   // Print error if setting socket option fails
        exit(EXIT_FAILURE);                     // Exit program if setsockopt fails
    }

    // Configure the server address structure
    address.sin_family = AF_INET;               // Use IPv4 address family
    address.sin_addr.s_addr = INADDR_ANY;       // Accept connections on any network interface
    address.sin_port = htons(PORT);             // Convert port number to network byte order

    // Bind the socket to the given IP and port
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");                  // Print error if binding fails
        exit(EXIT_FAILURE);                     // Exit program if bind fails
    }

    // Start listening for incoming client connections
    if (listen(server_fd, 3) < 0) {
        perror("listen");                       // Print error if listening fails
        exit(EXIT_FAILURE);                     // Exit program if listen fails
    }

    printf("Waiting for connections...\n");     // Inform the user that the server is ready and waiting

    // Accept a connection from a client (blocking call — waits until a client connects)
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");                       // Print error if accept fails
        exit(EXIT_FAILURE);                     // Exit program if accept fails
    }

    printf("Connection established!\n");
    printf("Client IP: %s\n", inet_ntoa(address.sin_addr));

    char client_name[BUFFER_SIZE] = {0};
    int name_read = read(new_socket, client_name, BUFFER_SIZE);
    if (name_read <= 0) {
        printf("Failed to read client name. Closing.\n");
        close(new_socket);
        close(server_fd);
        return 0;
    }
    client_name[strcspn(client_name, "\n")] = 0;
    printf("Client name set to: %s\n", client_name);

    // Enter a loop to continuously communicate with the connected client
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);         // Clear the buffer before reading new data

        int valread = read(new_socket, buffer, BUFFER_SIZE);  // Read data from the client
        if (valread <= 0) {                     // Check if the connection was closed or an error occurred
            printf("Connection closed.\n");     // Notify that the client disconnected
            break;                              // Exit the communication loop
        }

        buffer[strcspn(buffer, "\n")] = 0;  // usuń znak nowej linii z końca

  
        if (strcmp(buffer, "exit") == 0) {
            char *msg = "You disconnected.\n";
            send(new_socket, msg, strlen(msg), 0);
            printf("Client '%s' requested disconnection.\n", client_name);
            break;
        }

        printf("\033[1;32m%s: %s\033[0m\n", client_name, buffer);


        printf("\033[1;34mServer:\033[0m ");                     // Prompt server user to type a response
        fgets(buffer, BUFFER_SIZE, stdin);      // Read a message from standard input (keyboard)
        send(new_socket, buffer, strlen(buffer), 0); // Send the message back to the client
    }

    // Close the sockets when done
    close(new_socket);                          // Close the client socket
    close(server_fd);                           // Close the server socket
    return 0;                                   // End of the program
}
