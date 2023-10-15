#pragma once

void* threadConnection(void* arg);

/* 
 * cada nó contem:
 * nome de alguma fila
 * lista com sockets de todos consumers que estão inscritos nessa fila
 * ponteiro para o próximo nó, com outra fila e outra lista de sockets
*/

typedef struct{
    char* nomeFila;
    int* listaSockets;
    struct No* prox;
}No;