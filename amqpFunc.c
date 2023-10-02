#include "amqpFunc.h"

int readHeader(int connfd){
    char frame_type[8];
    char channel[16];
    char frame_size[32];

    ssize_t size;
    
    /* verificar se preciso enviar char[] como ponteiro */
    size = read(connfd, &frame_type, sizeof(frame_type));
    if (size == -1 || frame_type != "\x41\x4d\x51\x50\x00\x00\x09\x01") {
        return 0;
    }

    size = read(connfd, &channel, sizeof(channel));
    if (size == -1) {
        return 0;
    }

    size = read(connfd, &frame_size, sizeof(frame_size));
    if (size == -1) {
        return 0;
    }

    /*
    printf("AMQP Frame Header:\n");
    printf("Type: %02X\n", frame_type);
    printf("Channel: %02X\n", channel);
    printf("Size: %08X\n", frame_size);
    */

    return 1;    
}