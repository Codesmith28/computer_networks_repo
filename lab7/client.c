/* Computer Networks
   UDP client
   Team: Sarthak && Ritu
*/

#include "include/common.h"
#include "include/sr_arq.h"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct sockaddr_in sin;
    struct hostent *hp;
    char *host = NULL;
    int s, len;
    socklen_t sin_len = sizeof(sin);

    FILE *fp = NULL;
    char buffer[BUFFER_SIZE];

    // PORT and BUFFER_SIZE are defined in common.h

    if ((argc == 2) || (argc == 3)) {
        host = argv[1];
    } else {
        fprintf(stderr, "usage: client serverIP [download_filename(optional)]\n");
        exit(1);
    }

    // optionally providing a filename to receive to
    if (argc == 3) {
        fp = fopen(argv[2], "wb");
        if (fp == NULL) {
            fprintf(stderr, "Error opening output file\n");
            exit(1);
        }
    }

    hp = gethostname(host);
    if (!hp) {
        fprintf(stderr, "client: unknown host: %s\n", host);
        exit(1);
    } else {
        printf("Host %s found!\n", host);
    }

    // build a socket:
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy(&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(PORT);

    printf("Client will get data from to %s:%d.\n", argv[1], PORT);
    printf("To play the music, pipe the downlaod file to a player, e.g., ALSA, SOX, VLC: cat recvd_file.wav | vlc -\n");

    // send default GET to the server:
    strcpy(buffer, "GET");
    buffer[BUFFER_SIZE - 1] = '\0';
    len = strlen(buffer) + 1;
    // if (sendto(s, buffer, sizeof(buffer), 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
    //     perror("client: sendto");
    //     exit(1);
    // }

    while (1) {
    }
}

// int main(int argc, char *argv[]) {
//     int sockfd;
//     struct sockaddr_in server_addr;
//     socklen_t addr_len = sizeof(server_addr);

//     memset(&server_addr, 0, sizeof(server_addr));

//     sockfd = socket(AF_INET, SOCK_DGRAM, 0);
//     if (sockfd < 0) {
//         perror("Socket creation failed");
//         exit(EXIT_FAILURE);
//     }

//     server_addr.sin_family = AF_INET;
//     server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
//     server_addr.sin_port = htons(PORT);

//     receiver_init( );

//     for (int i = 0; i < 10; i++) {
//         receive_frame(sockfd, &server_addr, addr_len);
//         sleep(1);
//     }

//     close(sockfd);
//     return 0;
// }
