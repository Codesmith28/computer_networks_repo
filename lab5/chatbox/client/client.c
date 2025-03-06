#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_LINE 256

int main(int argc, char *argv[]) {
    char opt;
    if (argc < 5) {
        printf("Usage: %s -i <ip_address> -p <port>\n", argv[0]);
        exit(1);
    }

    struct sockaddr_in sin;
    char *ip_addr = NULL;
    
    char buffer[MAX_LINE];
    int s, len;
    int server_port = 0;

    while ((opt = getopt(argc, argv, "i:p:")) != -1) {
        switch (opt) {
            case 'i':
                ip_addr = optarg;
                break;
            case 'p':
                server_port = atoi(optarg);
                break;
            default:
                printf("Usage: %s -i <ip_address> -p <port>\n", argv[0]);
                exit(1);
        }
    }

    if (!ip_addr || server_port <= 0) {
        printf("Invalid IP address or port.\n");
        exit(1);
    }

    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    if (inet_pton(AF_INET, ip_addr, &sin.sin_addr) <= 0) {
        printf("Invalid IP address\n");
        exit(1);
    }
    sin.sin_port = htons(server_port);
    printf("Connecting to IP: %s\n", ip_addr);

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
        // send message
        printf("Enter message: ");
        if (!fgets(buffer, sizeof(buffer), stdin)) {
            perror("Error reading input");
            break;
        }
        buffer[strcspn(buffer, "\n")] = '\0';

        if (send(s, buffer, strlen(buffer), 0) < 0) {
            perror("Error sending message");
            break;
        }

        // receive response
        len = recv(s, buffer, sizeof(buffer) - 1, 0);
        if (len < 0) {
            perror("Error receiving message");
            break;
        } else if (len == 0) {
            printf("Server disconnected.\n");
            break;
        }

        buffer[len] = '\0';
        printf("Server response: %s\n", buffer);
    }

    close(s);
    return 0;
}