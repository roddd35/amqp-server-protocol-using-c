/* 
    OBS: para obter os pacotes com o AMQP RabbitMQ
    iniciar o servidor: systemctl start rabbitmq-server
                        systemctl enable rabbitmq-server
    amqp-declare-queue -q myqueue
    amqp-publish -r myqueue -b "Oie"
    amqp-consume -q myqueue cat
*/

#include "amqpFunc.h"
#include "auxFunctions.h"

void* threadConnection(void* arg);

uint64_t dTag = 1;
No* listaFilas;

/*-*-*- MAIN -*-*-*/
int main (int argc, char **argv) {
    /* Os sockets. */
    int listenfd, connfd;
    struct sockaddr_in servaddr;
    pid_t childpid;

    pthread_t threads[qtdTHREADS];
    long long int t = 0;
    
    if (argc != 2) {
        fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
        fprintf(stderr,"Vai rodar um servidor de echo na porta <Porta> TCP\n");
        exit(1);
    }

    /* Criação de um socket. */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket :(\n");
        exit(2);
    }

    /* Agora é necessário informar os endereços associados a este socket. */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(atoi(argv[1]));    /* quando rodar, usar a porta 5672 do AMQP */
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind :(\n");
        exit(3);
    }

    /* Socket que escutará por conexões. */
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen :(\n");
        exit(4);
    }

    printf("[Servidor no ar. Aguardando conexões na porta %s]\n",argv[1]);
    printf("[Para finalizar, pressione CTRL+c ou rode um kill ou killall]\n");
   
    listaFilas = iniciarLista(listaFilas);

	for (;;) {
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept :(\n");
            exit(5);
        }
      
        /* struct para utilizar threads, ao invés de FORk */
        struct ThreadArgs* args = (struct ThreadArgs*)malloc(sizeof(struct ThreadArgs));
        if (args == NULL) {
            perror("Falha na alocação de memória\n");
            close(connfd);
            continue;
        }
        args->connfd = connfd;

        /* chamar a função connection e incrementar uma thread a cada nova conexão */
        if ((childpid = pthread_create(&threads[t], NULL, threadConnection, args)) == 0)
            t++;
        else{
            printf("Não foi possível criar a thread!\n");
            close(connfd);
        }
    }
    close(listenfd);
    exit(0);
}

/*-*-*- OPERACOES DO AMQP USANDO THREADS -*-*-*/
void* threadConnection(void* arg){
    struct ThreadArgs* args = (struct ThreadArgs*)arg;
    int methodID;
    int connfd = args->connfd;
    char methodTxt[MAX_CHAR];
    char* queueName = NULL;
    char* publishQueue = NULL;
    uint8_t* message = NULL;
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

            /* registrar a fila na lista de filas */
            listaFilas = adicionaFila(listaFilas, listaFilas, queueName, connfd, NULL);
            /* imprimeFilas(listaFilas); */

            /* declarar a fila */
            queueDeclare(connfd, queueNameSize, queueName);
            closeChannel(connfd);
            closeChannelOk(connfd);
            closeConnection(connfd);
            close(connfd);
            printf("[Uma conexão fechada]\n");
            free(args);
        }   

        /* INSCREVER CONSUMIDOR NA FILA */
        else if(methodID == 20){
            uint8_t* ctag = generateCTAG();
            /* printf("%s\n", ctag); */

            /* receber o nome da fila */
            int queueNameSize = char2int(&methodTxt[13], 1);
            queueName = (char*)malloc(queueNameSize*sizeof(char));
            for(int i = 0; i < queueNameSize; i++)
                queueName[i] = (char)methodTxt[14+i];

            /* verificar se essa fila ja existe */
            if(!existeFila(listaFilas, queueName)){
                printf("Por favor, inscreva o consumidor em uma fila existente!\n");
                exit(0);
            }
            /* registrar a fila */
            listaFilas = adicionaFila(listaFilas, listaFilas, queueName, connfd, ctag);

            basicConsume(connfd, ctag);
        }

        /* PUBLICAR MENSAGEM NA FILA */
        else if(methodID == 40){
            /* separar o nome da fila e seu tamanho */
            char aux[MAX_CHAR];
            int exchageNameSize = char2int(&(methodTxt[13]), 1);
            int queueNameSize = char2int(&(methodTxt[14 + exchageNameSize]), 1);

            /* leitura do nome da fila */
            publishQueue = (char*)malloc(queueNameSize*sizeof(char));
            for(int i = 0; i < queueNameSize; i++)
                publishQueue[i] = (char)methodTxt[15+ exchageNameSize + i];

            /* verifica se a fila que estamos publicando existe */
            if(!existeFila(listaFilas, publishQueue)){
                printf("Por favor, publique as mensagens em filas declaradas!\n");
                exit(0);
            }

            /* leitura do segundo frame */
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

            int bodySize = char2LongLong(&aux[11], 8);

            size = read(connfd, aux, 7);
            if(size <= 0){
                close(connfd);
                exit(0);
            }

            /* leitura do terceiro frame */
            length = char2int(&aux[3], 4);
            size = read(connfd, aux+7, length+1);
            if(size <= 0){
                close(connfd);
                exit(0);
            }    

            if(bodySize != 0){
                message = (uint8_t*)malloc(sizeof(uint8_t) * bodySize);
                for(uint64_t j=0; j<bodySize; j++){
                    message[j] = (uint8_t)aux[7+j];
                }
            }

            closeChannel(connfd);
            listaFilas = basicDeliver(listaFilas, queueName, message, bodySize);
            basicAck(connfd);
            closeChannelOk(connfd);
            closeConnection(connfd);
            close(connfd);
            printf("[Uma conexão fechada]\n");
            free(args);
        }
    }
    /* free(args); */

    return NULL;
}