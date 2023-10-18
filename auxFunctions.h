#pragma once

#include "amqpFunc.h"

typedef struct cel{
    char* nomeFila;
    int* listaSockets;
    int qtdSockets;
    int maxConsumers;
    struct cel* prox;
}No;

uint8_t* generateCTAG();
void imprimeFilas(No* listaFilas);
int char2int(char* message, int size);
int existeFila(No* listaFilas, char* queueName);
int getConsumerSock(No* listaFilas, char* queueName);
No* iniciarLista(No* listaFilas);
No* realocaEspaco(No* listaFilas);
No* adicionaFila(No* listaFilas, No* primeiroLista, char* queueName, int connfd);