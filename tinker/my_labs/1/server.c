#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg) {
    perror(msg);
    exit(1);
}

// argc -> count of arguments
// argv -> array of arguments
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Port number not provided. Program terminated.\n");
        exit(1);
    }

    /*
     * sockfd -> file descriptor for the socket
     * newsockfd -> file descriptor for the accepted connection
     * portno -> port number on which the server accepts connections
     * n -> return value for the read() and write() calls
     * buffer -> buffer to store the message */

    int sockfd, newsockfd, portno, n;
    char buffer[255];

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t client_len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("Error opening socket.");
    }

    /* clear and set the server address structure */
    bzero((char *)&serv_addr, sizeof(serv_addr));
}
