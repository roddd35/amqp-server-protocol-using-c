#include "amqpFunc.h"
#include "connection.h"

void* connection(void* arg){
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    int connfd = args->connfd;

    int methodID;
    char methodTxt[MAX_CHAR];
    char* queueName = NULL;
    char* publishQueue = NULL;
    char* message = NULL;
    ssize_t size;

    printf("[Uma conexão aberta]\n");

    if(readHeader(connfd)){
        /* iniciar conexão e dependências */
        connectionStart(connfd);
        connectionTune(connfd);
        connectionOpen(connfd);
        channelOpen(connfd);

        /* ler qual método deve ser usado */
        size  = read(connfd, methodTxt, sizeof(methodTxt));
        if(size == -1)
            close(connfd);
        methodID = char2int(&methodTxt[9], 2);

        /* DECLARAR FILA */
        if(methodID == 10){
            /* receber o tamanho e nome da fila declarada */
            int queueNameSize = char2int(&methodTxt[13], 1);
            queueName = (char*)malloc(queueNameSize*sizeof(char));
            for(int i = 0; i < queueNameSize; i++)
                queueName[i] = (char)methodTxt[14+i];

            /* declarar a fila */
            queueDeclare(connfd, queueNameSize, queueName);
            closeChannel(connfd);
            closeChannelOk(connfd);
            closeConnection(connfd);
        }   

        /* INSCREVER CONSUMIDOR NA FILA */
        else if(methodID == 20){
            /* gerar uma consumer-tag pro write */
            uint8_t consumerTag[32];
            
            for (int i = 9; i < 31; i++) {
                consumerTag[i] = rand() % 26 + 'a'; // Gera um caractere aleatório de 'a' a 'z'
            }
            consumerTag[31] = '\xce';

            // Formate a consumer tag completa no vetor
            snprintf((char*)consumerTag, size, "amq.ctag-%s", (char*)&consumerTag[9]);
    
            printf("%s", consumerTag);
            
            /* fazer o mkfifo ou eventfd */
            basicDeliver(connfd, publishQueue, message);
            basicConsumeOk(connfd, queueName, consumerTag);
        }

        /* PUBLICAR MENSAGEM NA FILA */
        else if(methodID == 40){
            /* separar o nome da fila e seu tamanho */
            int queueNameSize = char2int(&methodTxt[14], 1);
            int messageSize = char2int(&methodTxt[46 + queueNameSize], 1);

            /* leitura do nome da fila */
            publishQueue = (char*)malloc(queueNameSize*sizeof(char));
            for(int i = 0; i < queueNameSize; i++)
                publishQueue[i] = (char)methodTxt[15+i];

            /* leitura da mensagem */
            message = (char*)malloc(messageSize*sizeof(char));
            for(int i = 0; i < messageSize; i++)
                message[i] = methodTxt[(47 + queueNameSize) + i];

            closeChannel(connfd);
            /* basicDeliver(connfd, publishQueue, message); */
            basicAck(connfd);
            closeChannelOk(connfd);
            closeConnection(connfd);
        }
    }
    printf("[Uma conexão fechada]\n");

    close(connfd);
    free(args);

    return NULL;
}