#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define SERVER_PORT 5432
#define MAX_LINE    256

int main( ) {
    struct sockaddr_in sin;
    char buf[MAX_LINE];
    socklen_t len;
    int s;
    char str[INET_ADDRSTRLEN];

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(SERVER_PORT);

    /* setup passive open */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }

    inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
    printf("Server is using address %s and port %d.\n", str, SERVER_PORT);

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("simplex-talk: bind");
        exit(1);
    } else
        printf("Server bind done.\n");

    /* wait for connection, then receive and print text */
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // receive message from the client:
        len = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
        if (len < 0) {
            perror("recvfrom error");
            continue;
        }

        // Null-terminate the received string
        buf[len] = '\0';

        // if the message's first 3 characters are not "GET" then print the message and continue
        if (strncmp(buf, "GET", 3) != 0) {
            printf("Received: %s", buf); // Print the non-GET message
            continue;
        }

        // Extract file name (skip "GET ")
        char *file_name = buf + 4;

        // Remove trailing newlines from the filename
        file_name[strcspn(file_name, "\r\n")] = '\0';

        printf("Request for file: %s\n", file_name);

        // Build the correct file path
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "./src_files/%s", file_name);

        FILE *fp = fopen(full_path, "rb");

        if (fp == NULL) {
            char *file_not_found = "FILE NOT FOUND";
            printf("File is not found in the server's src_files!\n");
            sendto(s, file_not_found, strlen(file_not_found), 0, (struct sockaddr *)&client_addr, client_len);
        } else {
            // Send file found confirmation
            char *file_found = "FILE FOUND";
            sendto(s, file_found, strlen(file_found), 0, (struct sockaddr *)&client_addr, client_len);

            // Send file content
            char file_buffer[MAX_LINE];
            size_t bytes_read;
            while ((bytes_read = fread(file_buffer, 1, sizeof(file_buffer), fp)) > 0) {
                sendto(s, file_buffer, bytes_read, 0, (struct sockaddr *)&client_addr, client_len);
                // small delay to prevent packet loss
                usleep(10000);
            }
            fclose(fp);

            //  EOF marker to signal transfer completion
            char *eof_marker = "EOF";
            sendto(s, eof_marker, strlen(eof_marker), 0, (struct sockaddr *)&client_addr, client_len);
        }
    }
}
