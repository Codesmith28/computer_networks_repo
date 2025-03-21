/* Computer Networks
   UDP server
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

// #define SERVER_PORT 5432
// #define BUF_SIZE    4096

int main(int argc, char *argv[]) {
    struct sockaddr_in sin;
    struct sockaddr_storage client_addr;
    char clientIP[INET_ADDRSTRLEN]; /* For IPv4 addresses */
    socklen_t client_addr_len;
    int len;
    int s;
    char *host = NULL;
    struct hostent *hp;

    /* Declarations for file(s) to be sent
       ...
    */
    char *filename = "Fight.mp4";

    /* For inserting delays, use nanosleep()
       struct timespec ... */
    struct timespec delay;
    delay.tv_sec = 1;
    delay.tv_nsec = 1000000;

    /* Modify the following code and use switches.
    For example, switches P and F for IP address and a filename (if streaming), respectively.
    */

    /*
     * ip address - i
     * filename - f
     * pot - p
     * payload size or BUF_SIZE - s
     * delay - d
     */

    int SERVER_PORT = 5432; // original hardcoded
    int BUF_SIZE = 4096;    // original hardcoded

    // optinal arguments:
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
                    delay.tv_nsec = 1000000;
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

    /* build address data structure and bind to all local addresses*/
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    /* If socket IP address specified, bind to it. */
    if (host == NULL) {
        sin.sin_addr.s_addr = INADDR_ANY;
    } else {
        hp = gethostbyname(host);
        if (!hp) {
            fprintf(stderr, "server: unknown host %s\n", host);
            exit(1);
        }
        memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    }
    sin.sin_port = htons(SERVER_PORT);

    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("server: bind");
        exit(1);
    } else {
        /* Add code to parse IPv6 addresses */
        inet_ntop(AF_INET, &(sin.sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Server successfully bound to %s:%d\n", clientIP, SERVER_PORT);
    }

    printf("Client needs to send \"GET\" to receive the file %s\n", filename); // Use filename here

    client_addr_len = sizeof(client_addr);

    /* Receive messages from clients*/
    printf("Waiting for client messages...\n");
    while ((len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&client_addr, &client_addr_len))) {
        inet_ntop(client_addr.ss_family, &(((struct sockaddr_in *)&client_addr)->sin_addr), clientIP, INET_ADDRSTRLEN);
        printf("Server got message from %s: %s [%d bytes]\n", clientIP, buf, len);

        // if the message if get then send the file else wait for the next message:
        if (strcmp(buf, "GET") != 0) {
            continue;
        }

        FILE *fp = fopen(filename, "rb"); // Open file here for each GET
        if (fp == NULL) {
            perror("Error opening file");
            exit(1);
        }

        printf("Sending file %s to client %s \n", filename, clientIP);

        // measure time to transmit the file:
        clock_t start, end;
        double cpu_time_used;
        start = clock( );

        while ((len = fread(buf, 1, BUF_SIZE, fp)) > 0) {
            /* Send to client */
            /* Add code to send file if the incoming message is GET */
            if (sendto(s, buf, len, 0, (struct sockaddr *)&client_addr, client_addr_len) < 0) {
                perror("server: sendto");
                exit(1);
            }
            printf("Sent %d bytes of %s\n", len, filename);

            if (nanosleep(&delay, NULL) < 0) {
                perror("nanosleep");
                exit(1);
            }

            memset(buf, 0, sizeof(buf));
        }
        // close the file pointer:
        fclose(fp);

        // measure time to transmit the file:
        end = clock( );
        cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
        printf("Time taken to transmit the file: %f seconds\n", cpu_time_used);

        /* Send BYE to signal termination */
        strcpy(buf, "BYE");
        if (sendto(s, buf, 3, 0, (struct sockaddr *)&client_addr, client_addr_len) < 0) {
            perror("server: sendto");
            exit(1);
        }
        printf("Sent BYE to %s\n", clientIP); // Confirm BYE sent
    }
}
