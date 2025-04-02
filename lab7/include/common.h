#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT        8080
#define BUFFER_SIZE 1024

typedef enum { FILE_REQUEST = 0, ACKNOWLEDGEMENT = 1, FILE_INFO_AND_DATA = 2, DATA = 3, FILE_NOT_FOUND = 4 } message_type;

// CLIENT
struct File_request {
    uint8_t type;
    uint8_t filename_size;
    char *filename;
    // char filename[ filename_size ];
};

struct ACK {
    uint8_t type;
    uint8_t num_sequences;
    uint16_t *sequence_no;
    // uint16_t sequence_no[num_sequences];
};

// SERVER
struct File_info_and_data {
    uint8_t type;
    uint16_t sequence_number;
    uint8_t filename_size;
    char *filename;
    // char filename[filename_size];
    
    uint32_t file_size;
    uint16_t block_size;
    char *data;
    // char data[block_size];
};

struct Data {
    uint8_t type;
    uint16_t sequence_number;
    uint16_t block_size;
    char *data;
    // char data[block_size];
};

struct File_not_found {
    uint8_t type;
    uint8_t filename_size;
    char *filename;
    // char filename[filename_size];
};

#endif