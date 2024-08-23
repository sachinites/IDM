#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>

#define HOSTNAME "mirror2.internetdownloadmanager.com"
#define FILEPATH "/idman642build20.exe"

void handle_http_request(const char *request, FILE *fp) {
    int sockfd;
    struct hostent *server;
    struct sockaddr_in server_addr;
    char response[4096];
    int bytes_received;
    int static content_length = -1;
    int header_end = 0;
    int header_received = 0;
    int total_bytes_received = 0;

    // Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sockfd < 0) {
        perror("Error opening socket");
        exit(EXIT_FAILURE);
    }

    // Resolve hostname to IP Address
    server = gethostbyname(HOSTNAME);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host\n");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    memset((char *)&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    memcpy((char *)&server_addr.sin_addr.s_addr, (char *)server->h_addr, server->h_length);
    server_addr.sin_port = htons(80); // HTTP Default Port

    // Connect to server
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Error connecting");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Send HTTP request
    if (write(sockfd, request, strlen(request)) < 0) {
        perror("Error writing to socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Read response
    while ((bytes_received = read(sockfd, response, sizeof(response) - 1)) > 0) {
        response[bytes_received] = '\0'; // Null-terminate the string

        if (strstr(request, "HEAD") != NULL) {
            // Extract Content-Length from HEAD response
            char *content_length_str = strstr(response, "Content-Length:");
            if (content_length_str != NULL) {
                sscanf(content_length_str, "Content-Length: %d", &content_length);
                printf("File size: %d bytes\n", content_length);
            }
            break;
        } else {
            // Write GET response to file
            if (!header_received) {
                char *header_end_marker = strstr(response, "\r\n\r\n");
                if (header_end_marker != NULL) {
                    header_end = header_end_marker - response + 4; // End of headers
                    fwrite(response + header_end, 1, bytes_received - header_end, fp);
                    total_bytes_received += bytes_received - header_end;
                    header_received = 1;
                } else {
                    fwrite(response, 1, bytes_received, fp);
                    total_bytes_received += bytes_received;
                }
            } else {
                fwrite(response, 1, bytes_received, fp);
                total_bytes_received += bytes_received;
            }
        }
        printf ("File downloaded : %d\n", (int)( (total_bytes_received * 100 ) / content_length));
    }

    if (bytes_received < 0) {
        perror("Error reading from socket");
    } else if (bytes_received == 0 && !header_received) {
        printf("Connection closed by server.\n");
    }

    close(sockfd);
}

int main(int argc, char **argv) {
    FILE *fp;
    char request[256];
    int content_length = -1;

    // Send HEAD request and get content length
    snprintf(request, sizeof(request), "HEAD %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
             FILEPATH, HOSTNAME);

    printf("Sending HEAD request\n");
    handle_http_request(request, NULL);

    // Open file in binary mode
    fp = fopen("idman642build20.exe", "wb");
    if (fp == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Send GET request to download the file
    snprintf(request, sizeof(request), "GET %s HTTP/1.1\r\nHost: %s\r\nConnection: close\r\n\r\n",
             FILEPATH, HOSTNAME);

    printf("Sending GET request\n");
    handle_http_request(request, fp);

    fclose(fp);
    return 0;
}
