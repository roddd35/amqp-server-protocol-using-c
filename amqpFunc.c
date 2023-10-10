#include "amqpFunc.h"

int char2int(char* message, int size){
    int i;
    int valor = 0;
    for(i = 0; i < size; i++){
        valor = valor << 8;
        valor += message[i];
    }

    return valor;
}

int readHeader(int connfd){
    char header[MAX_CHAR];
    ssize_t size;
    
    /* ler o header, dar um write nele pro usuario caso nao seja aceito */
    size = read(connfd, header, sizeof(header));
    if(size == -1 || strncmp("\x41\x4d\x51\x50\x00\x00\x09\x01", header, 8) != 0){
        write(connfd, "\x41\x4d\x51\x50\x00\x00\x09\x01", 8);
        close(connfd);
        return 0;
    }
    
    return 1;
}

void connectionStart(int connfd){
    char conn[MAX_CHAR];
    ssize_t size;

    /* escrever o Connection.Start */
    write(connfd, "\x01\x00\x00\x00\x00\x02\x00\x00\x0a\x00\x0a\x00\x09\x00\x00\x01" \
                  "\xdb\x0c\x63\x61\x70\x61\x62\x69\x6c\x69\x74\x69\x65\x73\x46\x00" \
                  "\x00\x00\xc7\x12\x70\x75\x62\x6c\x69\x73\x68\x65\x72\x5f\x63\x6f" \
                  "\x6e\x66\x69\x72\x6d\x73\x74\x01\x1a\x65\x78\x63\x68\x61\x6e\x67" \
                  "\x65\x5f\x65\x78\x63\x68\x61\x6e\x67\x65\x5f\x62\x69\x6e\x64\x69" \
                  "\x6e\x67\x73\x74\x01\x0a\x62\x61\x73\x69\x63\x2e\x6e\x61\x63\x6b" \
                  "\x74\x01\x16\x63\x6f\x6e\x73\x75\x6d\x65\x72\x5f\x63\x61\x6e\x63" \
                  "\x65\x6c\x5f\x6e\x6f\x74\x69\x66\x79\x74\x01\x12\x63\x6f\x6e\x6e" \
                  "\x65\x63\x74\x69\x6f\x6e\x2e\x62\x6c\x6f\x63\x6b\x65\x64\x74\x01" \
                  "\x13\x63\x6f\x6e\x73\x75\x6d\x65\x72\x5f\x70\x72\x69\x6f\x72\x69" \
                  "\x74\x69\x65\x73\x74\x01\x1c\x61\x75\x74\x68\x65\x6e\x74\x69\x63" \
                  "\x61\x74\x69\x6f\x6e\x5f\x66\x61\x69\x6c\x75\x72\x65\x5f\x63\x6c" \
                  "\x6f\x73\x65\x74\x01\x10\x70\x65\x72\x5f\x63\x6f\x6e\x73\x75\x6d" \
                  "\x65\x72\x5f\x71\x6f\x73\x74\x01\x0f\x64\x69\x72\x65\x63\x74\x5f" \
                  "\x72\x65\x70\x6c\x79\x5f\x74\x6f\x74\x01\x0c\x63\x6c\x75\x73\x74" \
                  "\x65\x72\x5f\x6e\x61\x6d\x65\x53\x00\x00\x00\x19\x72\x61\x62\x62" \
                  "\x69\x74\x40\x72\x6f\x64\x72\x69\x67\x6f\x2d\x56\x69\x72\x74\x75" \
                  "\x61\x6c\x42\x6f\x78\x09\x63\x6f\x70\x79\x72\x69\x67\x68\x74\x53" \
                  "\x00\x00\x00\x37\x43\x6f\x70\x79\x72\x69\x67\x68\x74\x20\x28\x63" \
                  "\x29\x20\x32\x30\x30\x37\x2d\x32\x30\x32\x32\x20\x56\x4d\x77\x61" \
                  "\x72\x65\x2c\x20\x49\x6e\x63\x2e\x20\x6f\x72\x20\x69\x74\x73\x20" \
                  "\x61\x66\x66\x69\x6c\x69\x61\x74\x65\x73\x2e\x0b\x69\x6e\x66\x6f" \
                  "\x72\x6d\x61\x74\x69\x6f\x6e\x53\x00\x00\x00\x39\x4c\x69\x63\x65" \
                  "\x6e\x73\x65\x64\x20\x75\x6e\x64\x65\x72\x20\x74\x68\x65\x20\x4d" \
                  "\x50\x4c\x20\x32\x2e\x30\x2e\x20\x57\x65\x62\x73\x69\x74\x65\x3a" \
                  "\x20\x68\x74\x74\x70\x73\x3a\x2f\x2f\x72\x61\x62\x62\x69\x74\x6d" \
                  "\x71\x2e\x63\x6f\x6d\x08\x70\x6c\x61\x74\x66\x6f\x72\x6d\x53\x00" \
                  "\x00\x00\x11\x45\x72\x6c\x61\x6e\x67\x2f\x4f\x54\x50\x20\x32\x34" \
                  "\x2e\x32\x2e\x31\x07\x70\x72\x6f\x64\x75\x63\x74\x53\x00\x00\x00" \
                  "\x08\x52\x61\x62\x62\x69\x74\x4d\x51\x07\x76\x65\x72\x73\x69\x6f" \
                  "\x6e\x53\x00\x00\x00\x06\x33\x2e\x39\x2e\x31\x33\x00\x00\x00\x0e" \
                  "\x50\x4c\x41\x49\x4e\x20\x41\x4d\x51\x50\x4c\x41\x49\x4e\x00\x00" \
                  "\x00\x05\x65\x6e\x5f\x55\x53\xce", 520);

    /* ler Connection.Start-Ok, enviado pelo cliente */
    size = read(connfd, conn, sizeof(conn));
    if(size == -1){
        close(connfd);
    }
}

void connectionTune(int connfd){
    char tune[MAX_CHAR];
    ssize_t size;

    /* Escrever o Connection.Tune */
    write(connfd, "\x01\x00\x00\x00\x00\x00\x0c\x00\x0a\x00\x1e\x07\xff\x00\x02\x00" \
                  "\x00\x00\x3c\xce", 20);

    /* Ler o Connection.Tune-Ok */
    size = read(connfd, tune, sizeof(tune));
    if(size == -1)
        close(connfd);
}

void connectionOpen(int connfd){
    char conn[MAX_CHAR];
    ssize_t size;

    /* Ler o Connection.Open */
    size = read(connfd, conn, sizeof(conn));
    if(size == -1)
        close(connfd);

    /* Confirmar a abertura de conexão com Connection.Open-Ok */
    write(connfd, "\x01\x00\x00\x00\x00\x00\x05\x00\x0a\x00\x29\x00\xce", 13);
}

void channelOpen(int connfd){
    char channel[MAX_CHAR];
    ssize_t size;
    
    /* Ler o request de abertura de canal (Channel.Open) */
    size = read(connfd, channel, sizeof(channel));
    if(size == -1)
        close(connfd);

    /* Confirmar a abertura de canal com Channel.Open-Ok */
    write(connfd, "\x01\x00\x01\x00\x00\x00\x08\x00\x14\x00\x0b\x00\x00\x00\x00\xce", 16);
}

void queueDeclare(int connfd, int queueNameSize, char* queueName){    
    /* concatenar a mensagem padrão com o nome da fila */
    uint8_t messageSize = 4 + queueNameSize + 1 + 8;

    uint8_t str1[] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x00,
                messageSize, 0x00, 0x32, 0x00, 0x0b};

    uint8_t* str2 = malloc((queueNameSize + 1) * sizeof(uint8_t));

    uint8_t str3[] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xce};

    str2[0] = queueNameSize;
    for (int i = 0; i < queueNameSize; i++) {
        str2[i + 1] = (uint8_t)queueName[i];
    }

    uint8_t res[11 + (queueNameSize + 1) + 9];

    memcpy(res, str1, sizeof(str1));
    memcpy(res + sizeof(str1), str2, queueNameSize + 1);
    memcpy(res + sizeof(str1) + queueNameSize + 1, str3, sizeof(str3));

    /* Escreve a confirmação do Queue.Declare-Ok */
    write(connfd, res, sizeof(res));
}

void closeChannel(int connfd){
    char chMessage[MAX_CHAR];
    ssize_t size;

    /* Ler o fechamento de canal */
    size = read(connfd, chMessage, sizeof(chMessage));
    if(size == -1)
        close(connfd);
}

void closeChannelOk(int connfd){
    /* Caso receba reply=200, escrever Channel.Close-Ok*/
    write(connfd, "\x01\x00\x01\x00\x00\x00\x04\x00\x14\x00\x29\xce", 12);
}

void closeConnection(int connfd){
    char connMessage[MAX_CHAR];
    ssize_t size;

    /* Ler o fechamento de conexao */
    size = read(connfd, connMessage, sizeof(connMessage));
    if(size == -1)
        close(connfd);
    
    /* Caso receba reply=200, escrever Connection.Close-Ok*/
    write(connfd, "\x01\x00\x00\x00\x00\x00\x04\x00\x0a\x00\x33\xce", 12);
}

void basicConsumeOk(int connfd, char* queueName, uint8_t* consumerTag){
    uint8_t str1[12] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x24, 0x00, 0x3c, 0x00, 0x15, 0x1f};
    uint8_t strMessage[44];

    memcpy(strMessage, str1, sizeof(str1));
    memcpy(strMessage + sizeof(str1), consumerTag, 32);
    
    /* escrever o consume-ok */
    write(connfd, strMessage, 44);
}

void basicDeliver(int connfd, char* queueName, char* message){
    /* definir o tamanho da mensagem e fila e alocar espaço para suas strings */
    uint8_t messageSize = strlen(message);
    uint8_t queueNameSize = strlen(queueName);
    uint8_t* strFila = malloc(sizeof(uint8_t) * (queueNameSize + 1));
    uint8_t* strMessage = malloc(sizeof(uint8_t) * (messageSize + 1));

    /* definir as strings de prefixos e sufixos padrões das mensagens */
    uint8_t str0[] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x38, 0x00, 0x3c, 0x00, 0x3c, 0x1f, 0x61, 0x6d, 0x71, 0x2e, 0x63, 0x74, 0x61, 0x67, 0x2d, 0x78, 0x51, 0x36, 0x53, 0x73, 0x73, 0x66, 0x7a, 0x67, 0x50, 0x43, 0x51, 0x48, 0x4e, 0x63, 0x4a, 0x36, 0x64, 0x45, 0x59, 0x31, 0x77, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
    uint8_t str1[] = {0xce};
    uint8_t str2[] = {0x03, 0x00, 0x01, 0x00, 0x00, 0x00};

    /* definir strings/tamanhos das respostas a serem escritas no write */
    uint8_t res1[53 + (queueNameSize + 1) + 1];
    uint8_t res2[] = {0x02, 0x00, 0x01, 0x00, 0x00, 0x00, 0x0f, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x10, 0x00, 0x01, 0xce};
    uint8_t res3[6 + (messageSize + 1) + 1];
    
    /* 1. frame */
    strFila[0] = queueNameSize;
    for(int i = 0; i < queueNameSize; i++)
        strFila[i + 1] = (uint8_t)queueName[i];

    memcpy(res1, str0, sizeof(str0));
    memcpy(res1 + sizeof(str0), strFila, queueNameSize + 1);
    memcpy(res1 + sizeof(str0) + queueNameSize + 1, str1, sizeof(str1));

    write(connfd, res1, sizeof(res1));
    
    /* 2. frame, contem o content-header */
    write(connfd, res2, 23);

    /* 3. frame, contem o content-body */
    strMessage[0] = messageSize;
    for(int i = 0; i < messageSize; i++)
        strMessage[i+1] = (uint8_t)message[i];

    memcpy(res3, str2, sizeof(str2));
    memcpy(res3 + sizeof(str2), strMessage, messageSize + 1);
    memcpy(res3 + sizeof(str2) + messageSize + 1, str1, sizeof(str1));

    write(connfd, res3, sizeof(res3));
}

void basicAck(int connfd){
    char ack[MAX_CHAR];
    ssize_t size;

    size = read(connfd, ack, sizeof(ack));
    if(size == -1)
        close(connfd);
}