#include "amqpFunc.h"
#include "auxFunctions.h"

int char2int(char* message, int size){
    int i;
    int valor = 0;
    for(i = 0; i < size; i++){
        valor = valor << 8;
        valor += message[i];
    }

    return valor;
}

uint8_t* generateCTAG(){
    uint8_t str1[] = {0x1f, 0x61, 0x6d, 0x71, 0x2e, 0x63, 0x74, 0x61, 0x67, 0x2d};
    uint8_t str2[] = {0xce};
    uint8_t* ctag = (uint8_t*)malloc(sizeof(uint8_t) * 33);
    uint8_t* ctagDigits = (uint8_t*)malloc(sizeof(uint8_t) * 22);

    srand(time(NULL));
    for(int i = 0; i < 22; i++){
        if(rand() % 2 == 0)
            ctagDigits[i] = rand() % 26 + 'a';
        else
            ctagDigits[i] = rand() % 26 + 'A';
    }

    memcpy(ctag, str1, sizeof(str1));
    memcpy(ctag + sizeof(str1), ctagDigits, 22);
    memcpy(ctag + sizeof(str1) + 22, str2, sizeof(str2));

    return ctag;
}

/* iniciamos permitindo 16 clientes em uma fila */
void iniciarLista(No* listaFilas){
    listaFilas->prox = NULL;
    listaFilas->nomeFila = NULL;
    listaFilas->listaSockets = (int*)malloc(sizeof(int) * 16);
    listaFilas->qtdSockets = 0;
}

void adicionaFila(No* listaFilas, char* queueName, int connfd){
    /* encontrou um nó vazio, preenche as informações */
    if(listaFilas->nomeFila == NULL){
        listaFilas->nomeFila = queueName;
        listaFilas->listaSockets[0] = connfd;
        listaFilas->qtdSockets = 1;
        listaFilas->prox = NULL;
        return;
    }

    /* encontrou a fila com mesmo nome */
    else if(strcmp(listaFilas->nomeFila, queueName) == 0){
        listaFilas->listaSockets[listaFilas->qtdSockets] = connfd;
        listaFilas->qtdSockets++;
        return;
    }

    /* procura por uma fila de mesmo nome ou ate encontrar uma vazia */
    else
        adicionaFila(listaFilas->prox, queueName, connfd);
}

void imprimeFilas(No* listaFilas){
    if(listaFilas->nomeFila != NULL){
        printf("fila: %s\n", listaFilas->nomeFila);
        imprimeFilas(listaFilas->prox);
    }
    return;
}