#pragma once

#include "amqpFunc.h"

typedef struct cel{
    char* nomeFila;
    int* listaSockets;
    int qtdSockets;
    struct cel* prox;
}No;

uint8_t* generateCTAG();
void iniciarLista(No* listaFilas);
void imprimeFilas(No* listaFilas);
void adicionaFila(No* listaFilas, char* queueName, int connfd);
int char2int(char* message, int size);