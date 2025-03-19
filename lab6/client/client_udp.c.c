/* Computer Networks
   UDP client
   Team: Sarthak && Ritu
*/

#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define SERVER_PORT 5432
#define BUF_SIZE    4096

int main(int argc, char *argv[]) {
    FILE *fp = NULL;
    struct hostent *hp;
    struct sockaddr_in sin;
    char *host;
    char buf[BUF_SIZE];
    int s;
    int len;

    /*
    Modify the following code and use switches.
    For example, switches I and P for IP address and Port, respectively.venv ma install karvani instruction pn lakhi aapje

i think i need to have a
    */

    if ((argc == 2) || (argc == 3)) {
        host = argv[1];
    } else {
        fprintf(stderr, "usage: client serverIP [download_filename(optional)]\n");
        exit(1);
    }

    if (argc == 3) {
        fp = fopen(argv[2], "wb");
        if (fp == NULL) {
            fprintf(stderr, "Error opening output file\n");
            exit(1);
        }
    }

    /* translate host name into peer's IP address */
    hp = gethostbyname(host);
    if (!hp) {
        fprintf(stderr, "client: unknown host: %s\n", host);
        exit(1);
    } else
        printf("Host %s found!\n", argv[1]);

    /* build address data structure */
    memset((char *)&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    memcpy((char *)&sin.sin_addr, hp->h_addr, hp->h_length);
    sin.sin_port = htons(SERVER_PORT);

    /* create socket */
    if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("client: socket");
        exit(1);
    }

    printf("Client will get data from to %s:%d.\n", argv[1], SERVER_PORT);
    printf("To play the music, pipe the downlaod file to a player, e.g., ALSA, SOX, VLC: cat recvd_file.wav | vlc -\n");

    /* send message to server */
    strcpy(buf, "GET");
    buf[BUF_SIZE - 1] = '\0';
    len = strlen(buf) + 1;
    if (sendto(s, buf, len, 0, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("Client: sendto()");
        return 0;
    }

    /* Get reply, display or store in a file.
       The client must be able to receive data without any size limits, and either display
       or store as specified by they user. Use a switch to implement it. */

    /* Instead of recv(), change to recvfrom() call for receiving data */
    while (1) {
        socklen_t sin_len = sizeof(sin);
        len = recvfrom(s, buf, sizeof(buf), 0, (struct sockaddr *)&sin, &sin_len);
        if (len < 0) {
            perror("Client: recvfrom()");
            return 0;
        }

        if (len == 3 && strncmp(buf, "BYE", 3) == 0) {
            printf("Transmission completed! End of file reached.\n");
            break;
        }

        // output or save:
        if (fp != NULL) {
            // write to the file:
            if (fwrite(buf, 1, len, fp) != len) {
                perror("Error writing to file");
                exit(1);
            }
        } else {
            // write to the stdout:
            if (fwrite(buf, 1, len, stdout) != len) {
                perror("Error writing to stdout");
                exit(1);
            }
            fflush(stdout);                                                  // Ensure data is sent to VLC immediately
            fprintf(stderr, "Received and wrote %d bytes to stdout\n", len); // Debug output
        }
    }

    if (fp != NULL) fclose(fp);
    return 0;
}