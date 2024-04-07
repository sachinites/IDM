/*
This code doesn't handle HTTP redirection or HTTPS. It's a basic example for HTTP file download.
You may need to enhance error handling and implement proper parsing of HTTP response to extract the file data.
Error checking and handling are simplified for brevity; in production code, you should handle errors more robustly.
This example assumes the file you're downloading is text-based. For binary files, additional handling may be required.
The code is synchronous, meaning it will block while waiting for the server's response. In a real-world scenario, you might 
want to make this asynchronous or handle multiple connections concurrently.
For HTTPS, you would need to use OpenSSL library or some other SSL/TLS library for secure communication.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFFER_SIZE 1024

// Function to create a socket, connect to the server, and send an HTTP GET request
int http_get_request(const char *hostname, const char *path, int port, char *response) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("Error opening socket");
        return -1;
    }

    // Get server details
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"Error, no such host\n");
        return -1;
    }

    // Initialize server address struct
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr, server->h_length);
    server_addr.sin_port = htons(port);

    // Connect to server
    if (connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) {
        perror("Error connecting");
        return -1;
    }

    // Send HTTP GET request
    char request[1000];
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, hostname);
    if (send(sockfd, request, strlen(request), 0) < 0) {
        perror("Error sending request");
        return -1;
    }

    // Receive response
    int bytes_received = recv(sockfd, response, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("Error receiving response");
        return -1;
    }

    // Close socket
    close(sockfd);

    return bytes_received;
}

int main() {
    char response[BUFFER_SIZE];

/* Download the file from GDrive using public link */
#if 0
    //const char *hostname = "drive.google.com";
    //const char *path = "/uc?id=YOUR_FILE_ID";
#endif

/* Download the file from Public Github Repo*/
#if 1
    const char *hostname = "raw.githubusercontent.com";
    const char *path = "/sachinites/IDM/main/README.md";
#endif

    int port = 80;

    int bytes_received = http_get_request(hostname, path, port, response);
    if (bytes_received > 0) {
        // Parse response and extract file data here
        printf("Response:\n%s\n", response);
    } else {
        printf("Failed to download file\n");
    }

    return 0;
}
