#pragma once

#define _GNU_SOURCE
#define LISTENQ 1
#define MAXLINE 4096
#define MAX_CHAR 1024
#define qtdTHREADS 150
#define MAXDATASIZE 100

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

struct ThreadArgs {
    int connfd;
};

typedef struct cel{
    char* nomeFila;
    int* listaSockets;
    int qtdSockets;
    int maxConsumers;
    uint8_t** cTag;
    uint64_t* dTag;
    struct cel* prox;
}No;

int readHeader(int connfd);
void basicAck(int connfd);
void channelOpen(int connfd);
void closeChannel(int connfd);
void closeChannelOk(int connfd);
void connectionTune(int connfd);
void connectionOpen(int connfd);
void connectionStart(int connfd);
void closeConnection(int connfd);
void basicConsume(int connfd, uint8_t* consumerTag);
void queueDeclare(int connfd, int queueNameSize, char* queueName);
No* basicPublish(No* listaFilas, char methodTxt[MAX_CHAR], int connfd);
No* basicDeliver(No* listaFilas, uint8_t* queueName, uint8_t* message, long long int bodySize);