/* 
    OBS: para obter os pacotes com o AMQP RabbitMQ
    iniciar o servidor: systemctl start rabbitmq-server
                        systemctl enable rabbitmq-server
    amqp-declare-queue -q myqueue
    amqp-publish -r myqueue -b "Oie"
    amqp-consume -q myqueue cat
*/

#include "amqpFunc.h"
#include "connection.h"

int main (int argc, char **argv) {
    /* Os sockets. */
    int listenfd;
    int connfd;
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
   
    /* struct para utilizar threads, ao invés de FORk */
    struct ThreadArgs* args = (struct ThreadArgs*)malloc(sizeof(struct ThreadArgs));
    if (args == NULL){
        perror("Falha na alocação de memória\n");
        exit(1);
    }
    
    
    /* reservamos espaço para 16 filas, como pedido para o protocolo */
    args->size = 0;
    args->queueList = (uint8_t**)malloc(sizeof(uint8_t*) * 16);
    for(int i = 0; i < 16; i++)
        args->queueList[i] = NULL;

    /* o servidor roda nesse loop */
	for (;;) {
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept :(\n");
            exit(5);
        }

        /* chamar a função connection e incrementar uma thread a cada nova conexão */
        args->connfd = connfd;
        if ((childpid = pthread_create(&threads[t], NULL, connection, args)) == 0)
            t++;
        else{
            printf("Não foi possível criar a thread!\n");
            close(connfd);
        }
    }
    close(listenfd);
    exit(0);
}