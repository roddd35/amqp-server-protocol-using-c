/* 
    OBS: para obter os pacotes com o AMQP RabbitMQ
    iniciar o servidor: systemctl start rabbitmq-server
                        systemctl enable rabbitmq-server
    amqp-declare-queue -q myqueue
    amqp-publish -r myqueue -b "Oie"
    amqp-consume -q myqueue cat
*/

#include "amqpFunc.h"

int main (int argc, char **argv) {
    /* Os sockets. */
    int listenfd, connfd;

    /* Informações sobre o socket (endereço e porta) ficam nesta struct */
    struct sockaddr_in servaddr;

    /* Retorno da função fork */
    pid_t childpid;
    
    int methodID;
    char methodTxt[MAX_CHAR];
    char* queueName = NULL;
    char* publishQueue = NULL;
    char* message = NULL;
    

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
   
	for (;;) {
        /* O socket inicial que foi criado é o socket que vai aguardar
         * pela conexão na porta especificada. Mas pode ser que existam
         * diversos clientes conectando no servidor. Por isso deve-se
         * utilizar a função accept. Esta função vai retirar uma conexão
         * da fila de conexões que foram aceitas no socket listenfd e
         * vai criar um socket específico para esta conexão. O descritor
         * deste novo socket é o retorno da função accept.
         */
        if ((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) == -1 ) {
            perror("accept :(\n");
            exit(5);
        }
      
        if ((childpid = fork()) == 0) {
            /**** PROCESSO FILHO ****/
            printf("[Uma conexão aberta]\n");

            close(listenfd);

            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 INÍCIO                        */
            /* ========================================================= */
            /* ========================================================= */
            /* TODO: É esta parte do código que terá que ser modificada
             * para que este servidor consiga interpretar comandos AMQP  */

            /* declarar uma fila */
            if(readHeader(connfd)){
                /* iniciar conexão e dependências */
                connectionStart(connfd);
                connectionTune(connfd);
                connectionOpen(connfd);
                channelOpen(connfd);

                /* ler qual método deve ser usado */
                read(connfd, methodTxt, sizeof(methodTxt));
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
                else if(methodID == 20)
                    basicConsume(connfd, queueName);

                /* PUBLICAR MENSAGEM NA FILA */
                else if(methodID == 40){
                    message = basicPublish(connfd, &publishQueue);
                    closeChannel(connfd);
                    basicDeliver(connfd, publishQueue, message);
                    basicAck(connfd);
                    closeChannelOk(connfd);
                    closeConnection(connfd);
                }

            }

            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 FIM                           */
            /* ========================================================= */
            /* ========================================================= */

            /* Finalizar o processo filho */
            printf("[Uma conexão fechada]\n");
            exit(0);
        }
        else
            /**** PROCESSO PAI ****/            
            close(connfd);
    }
    exit(0);
}