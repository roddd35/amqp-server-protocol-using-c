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

int readHeader(int connfd);
int char2int(char* message, int size);
void basicAck(int connfd);
void channelOpen(int connfd);
void closeChannel(int connfd);
void closeChannelOk(int connfd);
void connectionTune(int connfd);
void connectionOpen(int connfd);
void connectionStart(int connfd);
void closeConnection(int connfd);
void basicConsumeOk(int connfd, char* queueName, uint8_t* consumerTag);
void basicDeliver(int connfd, char* queueName, char* message);
void queueDeclare(int connfd, int queueNameSize, char* queueName);

struct ThreadArgs {
    int connfd;
};