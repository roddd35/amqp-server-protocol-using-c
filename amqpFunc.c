#include "amqpFunc.h"

int readHeader(int connfd){
    char amqpHeader[AMQP_header_size];
    read(connfd, amqpHeader, AMQP_header_size); /* ler mensagem de um socket */

    /* cabecalho obtido no wireshark */
    if(strncmp(amqpHeader, "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x08\x00\x45\x00" \
"\x00\x3c\x70\xe9\x40\x00\x40\x06\xcb\xd0\x7f\x00\x00\x01\x7f\x00" \
"\x00\x01\x80\x56\x16\x28\x45\xfb\x66\x19\xa0\xbd\x1c\x25\x80\x18" \
"\x02\x00\xfe\x30\x00\x00\x01\x01\x08\x0a\x2c\x7e\xf7\x3f\x2c\x7e" \
"\xf7\x3d\x41\x4d\x51\x50\x00\x00\x09\x01", AMQP_header_size) != 0){
        printf("Erro: cabeçalho inválido.\n");
        close(connfd);
        return 0;
    }
    
    return 1;
}

/* colocar isso em cima? junto ao readHeader? */
void sendResponse(int connfd){
    /* o que deve ser a resposta que o servidor envia pro cliente, depois de ler o header? */
    char response[12] = "oi";   
    write(connfd, response, sizeof(response)); 
}