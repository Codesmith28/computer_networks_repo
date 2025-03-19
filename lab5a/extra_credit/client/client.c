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

int main(int argc, char *argv[]) {
    FILE *fp;
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[MAX_LINE];
    int s;
    socklen_t len;

    if (argc == 2) {
        host = argv[1];
    } else {
        fprintf(stderr, "usage: %s host\n", argv[0]);
        exit(1);
    }

    /* translate host name into peer's IP address */
    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "%s: unknown host: %s\n", argv[0], host);
        exit(1);
    } else {
        printf("Client's remote host: %s\n", argv[1]);
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    bcopy(hp->h_addr, (char *)&sin.sin_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* active open */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    } else {
        printf("Client created socket.\n");
    }

    printf("Send a msg: \n");

    /* main loop: get and send lines of text */
    while (fgets(buf, sizeof(buf), stdin)) {
        size_t msg_len = strlen(buf);

        // Save the command for later use if it's a GET
        char command_copy[MAX_LINE];
        strcpy(command_copy, buf);

        // Send the message to the server
        if (sendto(s, buf, msg_len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
            perror("sendto");
            exit(1);
        }

        // Only process responses for GET commands
        if (strncmp(buf, "GET", 3) == 0) {
            // Extract the filename from the GET command
            char file_name[MAX_LINE];
            sscanf(command_copy, "GET %s", file_name);
            file_name[strcspn(file_name, "\r\n")] = '\0'; // Remove newline

            // Receive first response from the server
            len = sizeof(sin);
            int n = recvfrom(s, buf, sizeof(buf) - 1, 0, (struct sockaddr *)&sin, &len);
            if (n < 0) {
                perror("recvfrom");
                exit(1);
            }
            buf[n] = '\0'; // Null terminate the received message

            // Check if file exists
            if (strncmp(buf, "FILE NOT FOUND", 14) == 0) {
                printf("File not found on the server.\n");
            } else {
                printf("File found! Receiving file...\n");

                // Open the file for writing in binary mode
                fp = fopen(file_name, "wb");
                if (fp == NULL) {
                    perror("fopen");
                    continue;
                }

                // Receive file chunks until EOF marker is encountered
                while (1) {
                    len = sizeof(sin);
                    n = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &len);
                    if (n < 0) {
                        perror("recvfrom");
                        break;
                    }

                    // Check for EOF marker
                    if (n == 3 && strncmp(buf, "EOF", 3) == 0) {
                        break;
                    }

                    // Write received data to file
                    fwrite(buf, 1, n, fp);
                }

                fclose(fp);
                printf("File received successfully!\n");
            }
        }

        printf("Send a msg: \n");
    }

    close(s);
    return 0;
}
