/* Computer Networks
   UDP server
   Team: Sarthak && Ritu
*/

#include "include/sr_arq.h"
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

int main(int argc, char *argv[]) {
    struct sockaddr_in sin;
    struct sockaddr_storage client_addr;
    char client_ip[INET_ADDRSTRLEN];
    socklen_t client_addr_len;
    int len, s;

    char *host = NULL;
    char *file_name = NULL;
    struct hostent *hp;
    struct timespec delay = {0, 100000000}; // 0.1 seconds

    // PORT and BUFFER_SIZE are defined in common.h

    char buffer[BUFFER_SIZE];

    if (argc > 1) {
        int opts;
        while ((opts = getopt(argc, argv, "i:f:d:")) != -1) {
            switch (opts) {
                case 'i':
                    host = optarg;
                    break;
                case 'f':
                    file_name = optarg;
                    break;
                case 'd':
                    delay.tv_sec = atoi(optarg);
                    break;
                default:
                    fprintf(stderr, "usage: server -i serverIP -f filename -d delay\n");
                    exit(1);
            }
        }
    }

    // if no file provided:
    if (file_name == NULL) {
        fprintf(stderr, "usage: server -i serverIP -f filename -d delay\n");
        exit(1);
    }

    // Create a socket:
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("server: socket");
        exit(1);
    }

    // build the socket:
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    if (host == NULL) {
        sin.sin_addr.s_addr = INADDR_ANY;
    } else {
        hp = gethostbyname(host);
        if (!hp) {
            fprintf(stderr, "server: unknown host %s\n", host);
            exit(1);
        }
        memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
    }
    sin.sin_port = htons(PORT);

    // bind the socket to the port number:
    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("server: bind");
        exit(1);
    } else {
        inet_ntop(AF_INET, &sin.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("server: waiting for a connection on: %s\n", client_ip);
    }

    client_addr_len = sizeof(client_addr);

    printf("Client needs to send \"GET\" to receive the file: %s\n ", file_name);
    printf("Waiting for the client to send messages!\n");

    while (1) {
        // len = recvfrom(s, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (len < 0) {
            perror("server: recvfrom");
            exit(1);
        }

        inet_ntop(client_addr.ss_family, &((struct sockaddr_in *)&client_addr)->sin_addr, client_ip, client_addr_len);
        printf("Received message from %s: %s [%d bytes]\n", client_ip, buffer, len);
        if (strcmp(buffer, "GET") != 0) continue;
    }

    close(s);
    return 0;
}

// int main( ) {
//     int sockfd;
//     struct sockaddr_in server_addr, client_addr;
//     socklen_t addr_len = sizeof(client_addr);

//     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0) {
//         perror("Socket creation failed");
//         exit(EXIT_FAILURE);
//     }

//     memset(&server_addr, 0, sizeof(server_addr));
//     memset(&client_addr, 0, sizeof(client_addr));

//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = INADDR_ANY;
//     server_addr.sin_port = htons(PORT);

//     if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
//         perror("Bind failed");
//         exit(EXIT_FAILURE);
//     }

//     sender_init( );

//     for (int i = 0; i < 10; i++) {
//         sender_send(sockfd, &client_addr, addr_len);
//         sleep(1);
//     }

//     close(sockfd);
//     return 0;
// }
