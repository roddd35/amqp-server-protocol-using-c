/* Por Prof. Daniel Batista <batista@ime.usp.br>
 * Em 27/8/2023
 * 
 * Um código simples de um servidor de eco a ser usado como base para
 * o EP1. Ele recebe uma linha de um cliente e devolve a mesma linha.
 * Teste ele assim depois de compilar:
 * 
 * ./redes-servidor-exemplo-ep1 8000
 * 
 * Com este comando o servidor ficará escutando por conexões na porta
 * 8000 TCP (Se você quiser fazer o servidor escutar em uma porta
 * menor que 1024 você precisará ser root ou ter as permissões
 * necessáfias para rodar o código com 'sudo').
 *
 * Depois conecte no servidor via telnet. Rode em outro terminal:
 *  8
 * telnet 127.0.0.1 8000
 * 
 * Escreva sequências de caracteres seguidas de ENTER. Você verá que o
 * telnet exibe a mesma linha em seguida. Esta repetição da linha é
 * enviada pelo servidor. O servidor também exibe no terminal onde ele
 * estiver rodando as linhas enviadas pelos clientes.
 * 
 * Obs.: Você pode conectar no servidor remotamente também. Basta
 * saber o endereço IP remoto da máquina onde o servidor está rodando
 * e não pode haver nenhum firewall no meio do caminho bloqueando
 * conexões na porta escolhida.
 */

/* 
    OBS: para obter os pacotes com o AMQP RabbitMQ
    iniciar o servidor: systemctl start rabbitmq-server
                        systemctl enable rabbitmq-server
    amqp-declare-queue -q myqueue
    amqp-publish -r myqueue -b "Oie"
    amqp-consume -q myqueue cat
*/

#include "amqpFunc.h"

#define LISTENQ 1
#define MAXDATASIZE 100
#define MAXLINE 4096

int main (int argc, char **argv) {
    /* Os sockets. Um que será o socket que vai escutar pelas conexões
     * e o outro que vai ser o socket específico de cada conexão
     */
    int listenfd, connfd;
    /* Informações sobre o socket (endereço e porta) ficam nesta struct
     */
    struct sockaddr_in servaddr;
    /* Retorno da função fork para saber quem é o processo filho e
     * quem é o processo pai
     */
    pid_t childpid;
    /* Armazena linhas recebidas do cliente
     */
    /*char recvline[MAXLINE + 1];*/
    /* Armazena o tamanho da string lida do cliente
     */
    /*ssize_t n;*/
    

    if (argc != 2) {
        fprintf(stderr,"Uso: %s <Porta>\n",argv[0]);
        fprintf(stderr,"Vai rodar um servidor de echo na porta <Porta> TCP\n");
        exit(1);
    }

    /* Criação de um socket. É como se fosse um descritor de arquivo.
     * É possível fazer operações como read, write e close. Neste caso o
     * socket criado é um socket IPv4 (por causa do AF_INET), que vai
     * usar TCP (por causa do SOCK_STREAM), já que o AMQP funciona sobre
     * TCP, e será usado para uma aplicação convencional sobre a Internet
     * (por causa do número 0)
     */
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket :(\n");
        exit(2);
    }

    /* Agora é necessário informar os endereços associados a este
     * socket. É necessário informar o endereço / interface e a porta,
     * pois mais adiante o socket ficará esperando conexões nesta porta
     * e neste(s) endereços. Para isso é necessário preencher a struct
     * servaddr. É necessário colocar lá o tipo de socket (No nosso
     * caso AF_INET porque é IPv4), em qual endereço / interface serão
     * esperadas conexões (Neste caso em qualquer uma -- INADDR_ANY) e
     * qual a porta. Neste caso será a porta que foi passada como
     * argumento no shell (atoi(argv[1])). No caso do servidor AMQP,
     * utilize a porta padrão do protocolo que você deve descobrir
     * lendo a especificaçao dele ou capturando os pacotes de conexões
     * ao RabbitMQ com o Wireshark
     */
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(atoi(argv[1]));    /* quando rodar, usar a porta 5672 do AMQP */
    if (bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == -1) {
        perror("bind :(\n");
        exit(3);
    }

    /* Como este código é o código de um servidor, o socket será um
     * socket que escutará por conexões. Para isto é necessário chamar
     * a função listen que define que este é um socket de servidor que
     * ficará esperando por conexões nos endereços definidos na função
     * bind.
     */
    if (listen(listenfd, LISTENQ) == -1) {
        perror("listen :(\n");
        exit(4);
    }

    printf("[Servidor no ar. Aguardando conexões na porta %s]\n",argv[1]);
    printf("[Para finalizar, pressione CTRL+c ou rode um kill ou killall]\n");
   
    /* O servidor no final das contas é um loop infinito de espera por
     * conexões e processamento de cada uma individualmente
     */
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
      
        /* Agora o servidor precisa tratar este cliente de forma
         * separada. Para isto é criado um processo filho usando a
         * função fork. O processo vai ser uma cópia deste. Depois da
         * função fork, os dois processos (pai e filho) estarão no mesmo
         * ponto do código, mas cada um terá um PID diferente. Assim é
         * possível diferenciar o que cada processo terá que fazer. O
         * filho tem que processar a requisição do cliente. O pai tem
         * que voltar no loop para continuar aceitando novas conexões.
         * Se o retorno da função fork for zero, é porque está no
         * processo filho.
         */
        if ( (childpid = fork()) == 0) {
            /**** PROCESSO FILHO ****/
            printf("[Uma conexão aberta]\n");
            /* Já que está no processo filho, não precisa mais do socket
             * listenfd. Só o processo pai precisa deste socket.
             */
            close(listenfd);
         
            /* Agora pode ler do socket e escrever no socket. Isto tem
             * que ser feito em sincronia com o cliente. Não faz sentido
             * ler sem ter o que ler. Ou seja, neste caso está sendo
             * considerado que o cliente vai enviar algo para o servidor.
             * O servidor vai processar o que tiver sido enviado e vai
             * enviar uma resposta para o cliente (Que precisará estar
             * esperando por esta resposta) 
             */

            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 INÍCIO                        */
            /* ========================================================= */
            /* ========================================================= */
            /* TODO: É esta parte do código que terá que ser modificada
             * para que este servidor consiga interpretar comandos AMQP

             o que temos que fazer:
                - declaracao da fila (linhas 1 a 23 no wireshark)
                - conexao de vários clientes (publicar ou requisitar mensagem)
                - inscricao de cliente na fila e envio das mensagens da fila no esquema round robin
                - publicacao de uma mensagem na fila
                - desconexao de cliente

                -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
                
                - iniciar servidor, protocolo amqp e afins: linhas 1 a 16
                - declarar fila: linha 17. Processo acaba na linha 23
                - colocar clientes na fila: linhas 24 a 59
                - o cliente é colocado na linha 39 e 57. o restante são padrões do protocolo (comunicacao etc)
                - enviar mensagens pra fila (e passar para os clientes): linhas 60 em diante 
                - o publish ocorre nas linhas 75, 102, 129

                -*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

                - a ideia para TUDO (abrir fila, inscrever cliente, publicar mensagem) é a mesma
                2) envia o header do protocolo AMQP para iniciar a conexao
                3) abre a conexao AMQP
                4) abre o channel AMQP
                5) acao a ser feita (abrir fila, inscrever cliente, publicar mensagem)
                6) fechar o channel AMQP
                7) fechar a conexao AMQP
             */

            /* ler header */
            if(readHeader(connfd)){
                /* iniciar conexao */
                connectionStart(connfd);
            }

            /* ========================================================= */
            /* ========================================================= */
            /*                         EP1 FIM                           */
            /* ========================================================= */
            /* ========================================================= */

            /* Após ter feito toda a troca de informação com o cliente,
             * pode finalizar o processo filho
             */
            printf("[Uma conexão fechada]\n");
            exit(0);
        }
        else
            /**** PROCESSO PAI ****/
            /* Se for o pai, a única coisa a ser feita é fechar o socket
             * connfd (ele é o socket do cliente específico que será tratado
             * pelo processo filho)
             */
            close(connfd);
    }
    exit(0);
}