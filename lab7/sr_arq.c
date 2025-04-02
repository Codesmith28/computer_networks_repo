#include "include/sr_arq.h"

struct Frame sender_buffer[WINDOW_SIZE];
struct Frame receiver_buffer[WINDOW_SIZE];
int received[WINDOW_SIZE];

int sender_base = 0, next_frame_to_send = 0;
int receiver_base = 0;

void send_frame(int sock, struct sockaddr_in *client_addr, socklen_t addr_len, int seq) {
    struct Data frame;
    frame.type = DATA;
    frame.sequence_number = seq;
    frame.block_size = BUFFER_SIZE;
    snprintf(frame.data, BUFFER_SIZE, "Packet %d", seq);

    sendto(sock, &frame, sizeof(frame), 0, (struct sockaddr *)client_addr, addr_len);
    printf("Sender: Sent Frame %d\n", seq);
}

void handle_ack(int ack_seq) {
    printf("Sender: ACK received for Frame %d\n", ack_seq);
    if (ack_seq == sender_base) {
        sender_base++;
    }
}

void sender_init( ) {
    sender_base = 0;
    next_frame_to_send = 0;
}

void sender_send(int sock, struct sockaddr_in *client_addr, socklen_t addr_len) {
    if (next_frame_to_send < sender_base + WINDOW_SIZE) {
        send_frame(sock, client_addr, addr_len, next_frame_to_send);
        next_frame_to_send++;
    }
}

void receive_frame(int sock, struct sockaddr_in *server_addr, socklen_t addr_len) {
    struct Data frame;
    recvfrom(sock, &frame, sizeof(frame), 0, (struct sockaddr *)server_addr, &addr_len);

    if (frame.sequence_number >= receiver_base && frame.sequence_number < receiver_base + WINDOW_SIZE) {
        printf("Receiver: Received Frame %d\n", frame.sequence_number);
        receiver_buffer[frame.sequence_number % WINDOW_SIZE] = *(struct Frame *)&frame;
        received[frame.sequence_number % WINDOW_SIZE] = 1;

        while (received[receiver_base % WINDOW_SIZE]) {
            printf("Receiver: Delivering Frame %d\n", receiver_base);
            received[receiver_base % WINDOW_SIZE] = 0;
            receiver_base++;
        }

        send_ack(sock, server_addr, addr_len, frame.sequence_number);
    } else {
        printf("Receiver: Out-of-window frame %d discarded\n", frame.sequence_number);
    }
}

void send_ack(int sock, struct sockaddr_in *server_addr, socklen_t addr_len, int seq) {
    struct ACK ack;
    ack.type = ACKNOWLEDGEMENT;
    ack.num_sequences = 1;
    ack.sequence_no = malloc(sizeof(uint16_t));
    ack.sequence_no[0] = seq;

    sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)server_addr, addr_len);
    printf("Receiver: Sent ACK %d\n", seq);
    free(ack.sequence_no);
}

void receiver_init( ) {
    receiver_base = 0;
}
