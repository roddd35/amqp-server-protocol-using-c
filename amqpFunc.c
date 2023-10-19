#include "amqpFunc.h"
#include "auxFunctions.h"

/* Fazer a leitura do cabeçalho AMQP */
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

/* Função responsável por escrever a proposta de conexão */
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

/* Connection Tune */
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

/* Abrir Conexão */
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

/* Abrir Canal */
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

/* Declarar uma Fila */
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

/* Fechar Canal */
void closeChannel(int connfd){
    char chMessage[MAX_CHAR];
    ssize_t size;

    /* Ler o fechamento de canal */
    size = read(connfd, chMessage, sizeof(chMessage));
    if(size == -1)
        close(connfd);
}

/* Confirmar o fechamento de Canal */
void closeChannelOk(int connfd){
    /* Caso receba reply=200, escrever Channel.Close-Ok*/
    write(connfd, "\x01\x00\x01\x00\x00\x00\x04\x00\x14\x00\x29\xce", 12);
}

/* Fechar Conexão */
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

/* Escrever o Consume */
void basicConsume(int connfd, uint8_t* consumerTag){
    /* escrever o consume-ok */
    uint8_t res[44];
    uint8_t str1[] = {0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x24, 0x00, 0x3c, 0x00, 0x15};

    memcpy(res, str1, sizeof(str1));
    memcpy(res + sizeof(str1), consumerTag, 33);

    write(connfd, res, 44);
}

/* Publicar uma mensagem em uma fila */
No* basicPublish(No* listaFilas, char methodTxt[MAX_CHAR], int connfd){
    ssize_t size;
    uint8_t* publishQueue = NULL;
    uint8_t* message = NULL;
    char aux[MAX_CHAR];

    /* encontrar o tamanho da exchange e o nome da fila */
    uint8_t exchageNameSize = char2int(&(methodTxt[13]), 1);
    uint8_t queueNameSize = char2int(&(methodTxt[14 + exchageNameSize]), 1);

    /* leitura do nome da fila */
    publishQueue = (uint8_t*)malloc(queueNameSize * sizeof(uint8_t));
    for(int i = 0; i < queueNameSize; i++)
        publishQueue[i] = (uint8_t)methodTxt[15 + exchageNameSize + i];

    /* verifica se a fila que estamos publicando existe */
    if(!existeFila(listaFilas, (char*)publishQueue)){
        printf("Por favor, publique as mensagens em filas declaradas!\n");
        exit(0);
    }

    /* leitura do segundo frame (content-header) */
    size = read(connfd, aux, 7);
    if(size == -1){
        close(connfd);
        exit(0);
    }

    int length = char2int(&aux[3], 4);
    size = read(connfd, aux+7, length+1);
    if(size <= 0){
        close(connfd);
        exit(0);
    }

    uint64_t bodySize = char2LongLong(&aux[11], 8);

    /* leitura do terceiro frame (content-body) */
    size = read(connfd, aux, 7);
    if(size <= 0){
        close(connfd);
        exit(0);
    }

    length = char2int(&aux[3], 4);
    size = read(connfd, aux+7, length+1);
    if(size == -1){
        close(connfd);
        exit(0);
    }    

    if(bodySize != 0){
        message = (uint8_t*)malloc(sizeof(uint8_t) * bodySize);
        for(int i = 0; i < bodySize; i++){
            message[i] = (uint8_t)aux[7+i];
        }
    }

    /* channel.close */
    size = read(connfd, methodTxt, 7);
    if(size == -1){
        close(connfd);
        exit(0);
    }

    length = char2int(&methodTxt[3], 4);
    size = read(connfd, methodTxt+7, length + 1);

    listaFilas = basicDeliver(listaFilas, publishQueue, message, bodySize);

    return listaFilas;
}

/* Entregar a mensagem a um consumer */
No* basicDeliver(No* listaFilas, uint8_t* queueName, uint8_t* message, long long int bodySize){
    uint32_t length;
    No* noAux = listaFilas;

    /* buscar a fila a ser publicada a mensagem */
    while(noAux->nomeFila != NULL){
        if(strcmp((const char*)noAux->nomeFila, (const char*)queueName) == 0)
            break;
        noAux = noAux->prox;
    }

    int consumerSock = noAux->listaSockets[0];
    uint8_t* cTag = noAux->cTag[0];
    uint64_t dTag = noAux->dTag[0];

    /* rotacao da fila -> esquema round-robin */
    listaFilas = roundRobin(listaFilas, listaFilas, (char*)queueName);
    

    /* class(2) + method(2) + queueNameSize + queueName + messageCount(4) + consumerCount(4) */
    length = 16 + (uint8_t)sizeof(cTag) + (uint8_t)sizeof(queueName);
    length = htonl(length);

    /* 1. frame */
    uint8_t type1[] = {0x01};
    uint8_t channel1[] = {0x00, 0x01};
    uint8_t classs1[] = {0x00, 0x3c};
    uint8_t method1[] = {0x00, 0x3c};
    uint8_t sizeC[] = {(uint8_t)sizeof(cTag)};
    uint8_t redelivered[] = {0x00};
    uint8_t exchangeSize[] = {0x00};
    uint8_t queueNameSize[] = {(uint8_t)sizeof(queueName)};
    uint8_t end[] = {0xce};

    write(consumerSock, type1, 1);
    write(consumerSock, channel1, 2);
    write(consumerSock, (uint8_t*)&length, 4);
    write(consumerSock, classs1, 2);
    write(consumerSock, method1, 2);
    write(consumerSock, sizeC, 1);
    write(consumerSock, cTag, (int)sizeof(cTag));
    write(consumerSock, (uint8_t*)&dTag, 8);
    write(consumerSock, redelivered, 1);
    write(consumerSock, exchangeSize, 1);
    write(consumerSock, queueNameSize, 1);
    write(consumerSock, queueName, (uint8_t)sizeof(queueName));
    write(consumerSock, end, 1);


    /* 2. frame */
    length = (uint32_t)15;
    length = htonl(length);
    uint8_t type2[] = {0x02};
    uint8_t channel2[] = {0x00, 0x01};
    uint8_t classID[] = {0x00, 0x3c};
    uint8_t weight[] = {0x00, 0x00};
    uint8_t propertyFlags[] = {0x10, 0x00};
    uint8_t deliveryMode[] = {0x01};

    bodySize = htonll(bodySize);

    write(consumerSock, type2, 1);
    write(consumerSock, channel2, 2);
    write(consumerSock, (uint8_t*)&length, 4);
    write(consumerSock, classID, 2);
    write(consumerSock, weight, 2);
    write(consumerSock, (uint8_t*)&bodySize, 8);
    write(consumerSock, propertyFlags, 2);
    write(consumerSock, deliveryMode, 1);
    write(consumerSock, end, 1);


    /* 3. frame */
    length = (uint32_t)htonll(bodySize);
    length = htonl(length);
    uint8_t type3[] = {0x03};
    uint8_t channel3[] = {0x00, 0x01};

    write(consumerSock, type3, 1);
    write(consumerSock, channel3, 2);
    write(consumerSock, (uint8_t*)&length, 4);
    write(consumerSock, (uint8_t*)message, htonll(bodySize));
    write(consumerSock, end, 1);

    return listaFilas;
}

void basicAck(int connfd){
    char ack[MAX_CHAR];
    ssize_t size;

    size = read(connfd, ack, sizeof(ack));
    if(size == -1)
        close(connfd);
}