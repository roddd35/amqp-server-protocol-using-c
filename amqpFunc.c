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

void connectionStart(int connfd){
    write(connfd, "\x01\x00\x00\x00\x00\x01\x40\x00\x0a\x00\x0b\x00\x00\x01\x1c\x07" \
"\x70\x72\x6f\x64\x75\x63\x74\x53\x00\x00\x00\x0a\x72\x61\x62\x62" \
"\x69\x74\x6d\x71\x2d\x63\x07\x76\x65\x72\x73\x69\x6f\x6e\x53\x00" \
"\x00\x00\x06\x30\x2e\x31\x30\x2e\x30\x08\x70\x6c\x61\x74\x66\x6f" \
"\x72\x6d\x53\x00\x00\x00\x05\x4c\x69\x6e\x75\x78\x09\x63\x6f\x70" \
"\x79\x72\x69\x67\x68\x74\x53\x00\x00\x00\x49\x43\x6f\x70\x79\x72" \
"\x69\x67\x68\x74\x20\x28\x63\x29\x20\x32\x30\x30\x37\x2d\x32\x30" \
"\x31\x34\x20\x56\x4d\x57\x61\x72\x65\x20\x49\x6e\x63\x2c\x20\x54" \
"\x6f\x6e\x79\x20\x47\x61\x72\x6e\x6f\x63\x6b\x2d\x4a\x6f\x6e\x65" \
"\x73\x2c\x20\x61\x6e\x64\x20\x41\x6c\x61\x6e\x20\x41\x6e\x74\x6f" \
"\x6e\x75\x6b\x2e\x0b\x69\x6e\x66\x6f\x72\x6d\x61\x74\x69\x6f\x6e" \
"\x53\x00\x00\x00\x28\x53\x65\x65\x20\x68\x74\x74\x70\x73\x3a\x2f" \
"\x2f\x67\x69\x74\x68\x75\x62\x2e\x63\x6f\x6d\x2f\x61\x6c\x61\x6e" \
"\x78\x7a\x2f\x72\x61\x62\x62\x69\x74\x6d\x71\x2d\x63\x0c\x63\x61" \
"\x70\x61\x62\x69\x6c\x69\x74\x69\x65\x73\x46\x00\x00\x00\x3c\x1c" \
"\x61\x75\x74\x68\x65\x6e\x74\x69\x63\x61\x74\x69\x6f\x6e\x5f\x66" \
"\x61\x69\x6c\x75\x72\x65\x5f\x63\x6c\x6f\x73\x65\x74\x01\x1a\x65" \
"\x78\x63\x68\x61\x6e\x67\x65\x5f\x65\x78\x63\x68\x61\x6e\x67\x65" \
"\x5f\x62\x69\x6e\x64\x69\x6e\x67\x73\x74\x01\x05\x50\x4c\x41\x49" \
"\x4e\x00\x00\x00\x0c\x00\x67\x75\x65\x73\x74\x00\x67\x75\x65\x73" \
"\x74\x05\x65\x6e\x5f\x55\x53\xce", 328);
}