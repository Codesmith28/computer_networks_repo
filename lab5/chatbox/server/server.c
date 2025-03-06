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
    if (argc < 2) {
        printf("Usage: %s -p <port>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in sin;
    char buffer[MAX_LINE];
    socklen_t len;
    int s, new_s, server_port = 0;
    char str[INET_ADDRSTRLEN];

    while ((opt = getopt(argc, argv, "p:")) != -1) {
        switch (opt) {
            case 'p':
                printf("Port: %s\n", optarg);
                server_port = atoi(optarg);
                break;
            default:
                printf("Usage: %s -p <port>\n", argv[0]);
                exit(1);
        }
    }

    if (server_port <= 0) { // Validate port number
        printf("Invalid port number.\n");
        exit(1);
    }

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = inet_addr("10.1.58.104");
    sin.sin_port = htons(server_port);

    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }

    inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
    printf("Server is using address %s and port %d.\n", str, server_port);

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("simplex-talk: bind");
        close(s);
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
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sin.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Client connected from IP: %s\n", client_ip);

        while (1) {
            len = recv(new_s, buffer, sizeof(buffer) - 1, 0);
            if (len < 0) {
                perror("simplex-talk: recv");
                break;
            } else if (len == 0) {
                printf("Client disconnected.\n");
                break;
            }
            buffer[len] = '\0';
            printf("Received: %s\n", buffer);

            // get server's response
            printf("Enter response: ");
            if (!fgets(buffer, sizeof(buffer), stdin)) {
                perror("Error reading input");
                break;
            }
            buffer[strcspn(buffer, "\n")] = '\0';

            // send to client
            if (send(new_s, buffer, strlen(buffer), 0) < 0) {
                perror("Error sending message");
                break;
            }
        }

        close(new_s);
    }

    close(s);
    return 0;
}