#include "amqpFunc.h"
#include "auxFunctions.h"

int char2int(char* message, int size){
    int i;
    int val = 0;
    for(i = 0; i < size; i++){
        val = val << 8;
        val += message[i];
    }

    return val;
}

uint64_t char2LongLong(char* message, int size){
    uint64_t val = 0;
    for (int i = 0; i < size; i++) {
        /*shift de 8 bits*/
        val = val << 8;
        val += message[i];
    }

    return val;    
}

int existeFila(No* listaFilas, char* queueName){
    if(listaFilas->nomeFila == NULL)
        return 0;
    else if(strcmp(listaFilas->nomeFila, queueName) == 0)
        return 1;
    else return existeFila(listaFilas->prox, queueName);
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
No* iniciarLista(No* listaFilas){
    listaFilas = (No*)malloc(sizeof(No));
    listaFilas->prox = NULL;
    listaFilas->nomeFila = NULL;
    listaFilas->qtdSockets = 0;
    listaFilas->maxConsumers = 16;
    listaFilas->dTag = (uint64_t*)malloc(sizeof(uint64_t) * 16);
    listaFilas->cTag = (uint8_t**)malloc(sizeof(uint8_t*) * 16);
    for(int i = 0; i < 16; i++)
        listaFilas->cTag[i] = (uint8_t*)malloc(sizeof(uint8_t) * 33);

    listaFilas->listaSockets = (int*)malloc(sizeof(int) * 16);

    return listaFilas;
}

No* adicionaFila(No* listaFilas, No* primeiroLista, char* queueName, int connfd, uint8_t* ctag){
    /* cria fila nova */
    if(listaFilas->nomeFila == NULL){
        listaFilas->nomeFila = queueName;
        listaFilas->prox = iniciarLista(listaFilas->prox);
        return primeiroLista;
    }

    /* encontrou a fila com mesmo nome */
    else if(strcmp(listaFilas->nomeFila, queueName) == 0){
        listaFilas->listaSockets[listaFilas->qtdSockets] = connfd;
        listaFilas->cTag[listaFilas->qtdSockets] = ctag;
        if(listaFilas->qtdSockets == 0)
            listaFilas->dTag[listaFilas->qtdSockets] = (uint64_t)listaFilas->qtdSockets + 1;
        else
            listaFilas->dTag[listaFilas->qtdSockets] = listaFilas->dTag[listaFilas->qtdSockets - 1] + 1;
        listaFilas->qtdSockets++;

        if(listaFilas->qtdSockets == listaFilas->maxConsumers)
            listaFilas = realocaEspaco(listaFilas);
        return primeiroLista;
    }

    /* procura por uma fila de mesmo nome ou ate encontrar uma vazia */
    else
        return adicionaFila(listaFilas->prox, primeiroLista, queueName, connfd, ctag);
}

No* realocaEspaco(No* listaFilas){
    int* auxConsumers;
    uint8_t** auxCTAG;
    uint64_t* auxDTAG;

    auxConsumers = (int*)malloc(sizeof(int) * (2 * listaFilas->maxConsumers));
    auxCTAG = (uint8_t**)malloc(sizeof(uint8_t*) * (2 * listaFilas->maxConsumers));
    auxDTAG = (uint64_t*)malloc(sizeof(uint64_t) * (2 * listaFilas->maxConsumers));

    for(int i = 0; i < listaFilas->qtdSockets; i++){
        auxConsumers[i] = listaFilas->listaSockets[i];
        auxDTAG[i] = listaFilas->dTag[i];
        auxCTAG[i] = listaFilas->cTag[i];
    }

    free(listaFilas->listaSockets);
    free(listaFilas->cTag);
    free(listaFilas->dTag);

    listaFilas->listaSockets = auxConsumers;
    listaFilas->cTag = auxCTAG;
    listaFilas->dTag = auxDTAG;

    listaFilas->maxConsumers = 2 * listaFilas->maxConsumers;
    return listaFilas;
}

No* roundRobin(No* listaFilas, No* primeiroLista, char* queueName){
    int sockAux;
    uint8_t* cTagAux;
    uint64_t dTagAux;

    if(strcmp(listaFilas->nomeFila, queueName) == 0){
        sockAux = listaFilas->listaSockets[0];
        cTagAux = listaFilas->cTag[0];
        dTagAux = listaFilas->dTag[0];

        for(int i = 0; i < listaFilas->qtdSockets - 1; i++){
            listaFilas->listaSockets[i] = listaFilas->listaSockets[i+1];
            listaFilas->cTag[i] = listaFilas->cTag[i+1];
            listaFilas->dTag[i] = listaFilas->dTag[i+1];
        }
        listaFilas->listaSockets[listaFilas->qtdSockets - 1] = sockAux;
        listaFilas->cTag[listaFilas->qtdSockets - 1] = cTagAux;
        listaFilas->dTag[listaFilas->qtdSockets - 1] = dTagAux;

        return primeiroLista;
    }
    else return roundRobin(listaFilas->prox, primeiroLista, queueName);
}

void imprimeFilas(No* listaFilas){
    if(listaFilas->nomeFila != NULL){
        printf("fila: %s\n", listaFilas->nomeFila);
        imprimeFilas(listaFilas->prox);
    }
    return;
}