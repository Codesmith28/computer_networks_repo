#ifndef SR_ARQ_H
#define SR_ARQ_H

#include "common.h"

#define MAX_SEQ     7
#define WINDOW_SIZE ((MAX_SEQ + 1) / 2)

struct Frame {
    int seq;
    char data[BUFFER_SIZE]; // Use BUFFER_SIZE from common.h
    int is_ack;
};

// Server (Sender) Functions
void send_frame(int sock, struct sockaddr_in *client_addr, socklen_t addr_len, int seq);
void handle_ack(int ack_seq);
void sender_init( );
void sender_send(int sock, struct sockaddr_in *client_addr, socklen_t addr_len);

// Client (Receiver) Functions
void receive_frame(int sock, struct sockaddr_in *server_addr, socklen_t addr_len);
void send_ack(int sock, struct sockaddr_in *server_addr, socklen_t addr_len, int seq);
void receiver_init( );

#endif // SR_ARQ_H
