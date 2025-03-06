#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LINE 256

int main(int argc, char *argv[]) {
    char opt;
    if (argc < 5) {
        printf("Usage: %s -h <host> -p <port>\n", argv[0]);
        exit(1);
    }

    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buffer[MAX_LINE];
    int s, len;
    char *file_receive = NULL;
    int server_port;

    while ((opt = getopt(argc, argv, "h:p:f:")) != -1) {
        switch (opt) {
            case 'h':
                host = optarg;
                break;
            case 'p':
                server_port = atoi(optarg);
                break;
            case 'f':
                file_receive = optarg;
                break;
            default:
                printf("Usage: %s -h <host> -p <port>\n", argv[0]);
                exit(1);
         }
    }

    if (file_receive == NULL) {
        printf("Using the default sample_receive.txt file to receive\n");
        file_receive = "./sample_receive.txt";
    }

    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
        exit(1);
    }
    printf("Client's remote host: %s\n", host);

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(server_port);

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }
    printf("Client created socket.\n");

    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("simplex-talk: connect");
        close(s);
        exit(1);
    }
    printf("Client connected.\n");

    while (1) {
        printf("Enter message (or 'GET' to receive file): ");
        fgets(buffer, sizeof(buffer), stdin);
        buffer[strcspn(buffer, "\n")] = '\0';

        if (send(s, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending message");
            break;
        }

        if (strcmp(buffer, "GET") == 0) {
            printf("Requesting file from server...\n");

            // Open file for writing
            FILE *to_receive = fopen(file_receive, "wb");
            if (to_receive == NULL) {
                perror("Error opening file");
                continue;
            }

            // Receive file size first
            long file_size;
            if (recv(s, &file_size, sizeof(file_size), 0) <= 0) {
                perror("Error receiving file size");
                fclose(to_receive);
                continue;
            }

            printf("File size to receive: %ld bytes\n", file_size);

            // Receive file data
            long bytes_received = 0;
            while (bytes_received < file_size) {
                len = recv(s, buffer, MAX_LINE, 0);
                if (len <= 0) {
                    perror("Error receiving file");
                    break;
                }

                fwrite(buffer, 1, len, to_receive);
                bytes_received += len;
            }

            printf("File received successfully.\n");
            fclose(to_receive);
            continue;
        }
    }

    close(s);
    return 0;
}