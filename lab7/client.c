#include "include/sr_arq.h"


int main(int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    memset(&server_addr, 0, sizeof(server_addr));

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("0.0.0.0");
    server_addr.sin_port = htons(PORT);

    receiver_init( );

    for (int i = 0; i < 10; i++) {
        receive_frame(sockfd, &server_addr, addr_len);
        sleep(1);
    }

    close(sockfd);
    return 0;
}
