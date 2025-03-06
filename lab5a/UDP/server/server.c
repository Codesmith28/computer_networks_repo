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
    if (argc < 3) {
        printf("Usage: %s -p <port> -f <filepath>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in sin;
    char buffer[MAX_LINE];
    socklen_t addr_len;
    int s, server_port = 0;
    char str[INET_ADDRSTRLEN];
    char *file_send = NULL;

    while ((opt = getopt(argc, argv, "p:f:")) != -1) {
        switch (opt) {
            case 'p':
                server_port = atoi(optarg);
                break;
            case 'f':
                file_send = optarg;
                break;
            default:
                printf("Usage: %s -p <port> -f <filepath>\n", argv[0]);
                exit(1);
        }
    }

    if (server_port == 0) {
        fprintf(stderr, "Invalid arguments!\n");
        printf("Usage: %s -p <port> -f <file>\n", argv[0]);
        exit(1);
    }

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    // SOCK_DGRAM for UDP
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        exit(1);
    }

    inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
    printf("Server is using address %s and port %d.\n", str, server_port);

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("bind");
        exit(1);
    }
    printf("Server bind done.\n");

    while (1) {
        addr_len = sizeof(sin);
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int len = recvfrom(s, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&client_addr, &client_len);
        if (len < 0) {
            perror("recvfrom");
            continue;
        }
        buffer[len] = '\0';
        printf("Received: %s\n", buffer);

        if (strcmp(buffer, "GET") == 0) {
            FILE *to_send = fopen(file_send, "rb");
            if (to_send == NULL) {
                const char *error_msg = "File Not Found";
                sendto(s, error_msg, strlen(error_msg), 0, (struct sockaddr *)&client_addr, client_len);
                printf("File not found, sent error message to client.\n");
                continue;
            }

            // Get file size
            fseek(to_send, 0, SEEK_END);
            long file_size = ftell(to_send);
            rewind(to_send);
            printf("Sending file of size: %ld bytes\n", file_size);

            // First send a success message
            const char *success_msg = "FILE_FOUND";
            sendto(s, success_msg, strlen(success_msg), 0, (struct sockaddr *)&client_addr, client_len);

            // Send file size
            sendto(s, &file_size, sizeof(file_size), 0, (struct sockaddr *)&client_addr, client_len);

            // Send file data
            size_t bytes_read;
            while ((bytes_read = fread(buffer, 1, MAX_LINE, to_send)) > 0) {
                if (sendto(s, buffer, bytes_read, 0, (struct sockaddr *)&client_addr, client_len) < 0) {
                    perror("Error sending file data");
                    break;
                }
            }

            fclose(to_send);
            printf("File sent successfully.\n");

            // send BYE message after file is sent:
            const char *bye_msg = "BYE";
            sendto(s, bye_msg, strlen(bye_msg), 0, (struct sockaddr *)&client_addr, client_len);
        }
    }

    close(s);
    return 0;
}
