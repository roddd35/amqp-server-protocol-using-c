#pragma once

#include "amqpFunc.h"

typedef struct cel{
    char* nomeFila;
    int* listaSockets;
    int qtdSockets;
    struct cel* prox;
}No;

uint8_t* generateCTAG();
void imprimeFilas(No* listaFilas);
int char2int(char* message, int size);
No* iniciarLista(No* listaFilas);
No* adicionaFila(No* listaFilas, No* primeiroLista, char* queueName, int connfd);