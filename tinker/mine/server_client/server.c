#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_PENDING 5
#define MAX_LINE    256

void error_func(const char *err_msg) {
    perror(err_msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    char opt;
    if (argc < 2) {
        printf("Usage: %s -p <port>\n", argv[0]);
        exit(1);
    }

    /*define the inputs required:*/
    struct sockaddr_in sin;
    char buffer[MAX_LINE];
    socklen_t len;
    int s, new_s, server_port;
    char str[INET_ADDRSTRLEN];

    while (opt = getopt(argc, argv, "p:"), opt != -1) {
        switch (opt) {
            case 'p':
                printf("Port: %s\n", optarg);
                break;
            default:
                printf("Usage: %s -p <port>\n", argv[0]);
                exit(1);
        }
    }

    /* build address data structure */
    bzero((char *)&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;
    sin.sin_port = htons(server_port);

    /* setup passive open */
    if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        perror("simplex-talk: socket");
        exit(1);
    }

    /*convert numeric addr to human readable form*/
    inet_ntop(AF_INET, &(sin.sin_addr), str, INET_ADDRSTRLEN);
    printf("Server is using address %s and port %d.\n", str, server_port);

    /*bind the server to the address and port*/
    if ((bind(s, (struct sockaddr *)&sin, sizeof(sin))) < 0) {
        perror("simplex-talk: bind");
        exit(1);
    } else {
        printf("Server bind done.\n");
    }

    /*listen to the socket*/
    listen(s, MAX_PENDING);

    /*wait for connection, then receive and print text*/
    while (1) {
        if ((new_s = accept(s, (struct sockaddr *)&sin, &len)) < 0) {
            perror("simplex-talk: accept");
            exit(1);
        }

        printf("Server Listening.\n");
        while ((len = recv(new_s, buffer, sizeof(buffer), 0))) fputs(buffer, stdout);

        close(new_s);
    }
}
