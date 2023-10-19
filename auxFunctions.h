#pragma once

#include "amqpFunc.h"

uint8_t* generateCTAG();
uint64_t char2LongLong(char* message, int size);
void imprimeFilas(No* listaFilas);
int char2int(char* message, int size);
int existeFila(No* listaFilas, char* queueName);
No* iniciarLista(No* listaFilas);
No* realocaEspaco(No* listaFilas);
No* roundRobin(No* listaFilas, No* primeiroLista, char* queueName);
No* adicionaFila(No* listaFilas, No* primeiroLista, char* queueName, int connfd, uint8_t* ctag);