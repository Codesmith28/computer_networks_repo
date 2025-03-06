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

#define MAX_PENDING 5
#define MAX_LINE    256

int main(int argc, char *argv[]) {
    char opt;
    if (argc < 5) {
        printf("Usage: %s -p <port> %s -f <filepath> \n", argv[0], argv[1]);
        exit(1);
    }

    struct sockaddr_in sin;
    char buffer[MAX_LINE];
    socklen_t len;
    int s, new_s, server_port;
    char str[INET_ADDRSTRLEN];
    char *file_send = NULL;

    while ((opt = getopt(argc, argv, "p:f:")) != -1) {
        switch (opt) {
            case 'p':
                printf("Port: %s\n", optarg);
                server_port = atoi(optarg);
                break;
            case 'f':
                printf("File: %s\n", optarg);
                file_send = optarg;
                break;
            default:
                printf("Usage: %s -p <port> %s -f <filepath> \n", argv[0], argv[1]);
                exit(1);
        }
    }

    // if (file_send == NULL) {
    //     printf("Using the default sample.txt file to send\n");
    //     file_send = "./sample.txt";
    // }

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }

    inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
    printf("Server is using address %s and port %d.\n", str, server_port);

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("simplex-talk: bind");
        exit(1);
    }
    printf("Server bind done.\n");

    listen(s, MAX_PENDING);

    while (1) {
        len = sizeof(sin);
        new_s = accept(s, (struct sockaddr *)&sin, &len);
        if (new_s < 0) {
            perror("simplex-talk: accept");
            continue;
        }

        printf("Server Listening...\n");
        while ((len = recv(new_s, buffer, sizeof(buffer) - 1, 0)) > 0) {
            buffer[len] = '\0';
            printf("Received: %s\n", buffer);

            if (strcmp(buffer, "GET") == 0) {
                printf("GET request received. Sending file...\n");
                FILE *to_send = fopen(file_send, "r");
                if (to_send == NULL) {
                    perror("Error opening file");
                    break;
                }

                // Get file size
                fseek(to_send, 0, SEEK_END);
                long file_size = ftell(to_send);
                rewind(to_send);
                printf("File size: %ld bytes\n", file_size);

                // Send file size first
                if (send(new_s, &file_size, sizeof(file_size), 0) < 0) {
                    perror("Error sending file size");
                    fclose(to_send);
                    break;
                }

                // Send file data
                size_t bytes_read;
                while ((bytes_read = fread(buffer, 1, MAX_LINE, to_send)) > 0) {
                    if (send(new_s, buffer, bytes_read, 0) < 0) {
                        perror("Error sending file");
                        break;
                    }
                }

                fclose(to_send);
                printf("File sent successfully.\n");
            }
        }
        printf("Client disconnected.\n");
        close(new_s);
    }
    close(s);
    return 0;
}