/* Computer Networks
   UDP client
   Team: Sarthak && Ritu
*/

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    struct sockaddr_in sin;
    struct sockaddr_storage client_addr;
    char clientIP[INET_ADDRSTRLEN];
    socklen_t client_addr_len;
    int len, s;
    char *host = NULL;
    char *filename = "Tatakae.mp4";
    struct hostent *hp;
    struct timespec delay = {0, 1000000}; // 1ms delay

    int SERVER_PORT = 5432;
    int BUF_SIZE = 4096;

    // Parse command-line arguments (unchanged)
    if (argc > 1) {
        int opts;
        while ((opts = getopt(argc, argv, "i:f:p:s:d:")) != -1) {
            switch (opts) {
                case 'i':
                    host = optarg;
                    break;
                case 'f':
                    filename = optarg;
                    break;
                case 'p':
                    SERVER_PORT = atoi(optarg);
                    break;
                case 's':
                    BUF_SIZE = atoi(optarg);
                    break;
                case 'd':
                    delay.tv_sec = atoi(optarg);
                    break;
                default:
                    fprintf(stderr, "usage: server -i serverIP -f filename -p port -s payload_size -d delay\n");
                    exit(1);
            }
        }
    }

    char buf[BUF_SIZE];
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("server: socket");
        exit(1);
    }

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
    sin.sin_port = htons(SERVER_PORT);

    if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("server: bind");
        exit(1);
    } else {
        inet_ntop(AF_INET, &sin.sin_addr, clientIP, INET_ADDRSTRLEN);
        printf("Server bound to %s:%d\n", clientIP, SERVER_PORT);
    }

    printf("Client needs to send \"GET\" to receive the file %s\n", filename);
    client_addr_len = sizeof(client_addr);

    printf("Waiting for client messages...\n");
    while (1) {
        len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (len < 0) {
            perror("recvfrom");
            continue;
        }

        inet_ntop(client_addr.ss_family, &(((struct sockaddr_in *)&client_addr)->sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Server got message from %s: %s [%d bytes]\n", clientIP, buf, len);

        if (strcmp(buf, "GET") != 0) {
            continue;
        }

        pid_t pid = fork( );
        if (pid < 0) {
            perror("fork");
            continue;
        }

        if (pid == 0) { // Child process
            FILE *fp = fopen(filename, "rb");
            if (fp == NULL) {
                perror("Error opening file");
                exit(1);
            }

            printf("Child process sending file %s to %s\n", filename, clientIP);
            clock_t start = clock( );

            while ((len = fread(buf, 1, BUF_SIZE, fp)) > 0) {
                if (sendto(s, buf, len, 0, (struct sockaddr *)&client_addr, client_addr_len) < 0) {
                    perror("sendto");
                    exit(1);
                }
                nanosleep(&delay, NULL);
            }

            fclose(fp);
            double cpu_time_used = ((double)(clock( ) - start)) / CLOCKS_PER_SEC;
            printf("Time taken to transmit file: %f seconds\n", cpu_time_used);

            strcpy(buf, "BYE");
            sendto(s, buf, 3, 0, (struct sockaddr *)&client_addr, client_addr_len);
            printf("Sent BYE to %s\n", clientIP);
            exit(0); // Child exits after handling the client
        }
        // Parent continues to listen for new requests
    }
    close(s); // Never reached in this example
    return 0;
}
