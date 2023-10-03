#pragma once

#define _GNU_SOURCE
#define LISTENQ 1
#define MAXLINE 4096
#define MAX_CHAR 1024
#define MAXDATASIZE 100

#include <time.h>
#include <stdio.h>
#include <errno.h>
#include <netdb.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int readHeader(int connfd);
void channelOpen(int connfd);
void queueDeclare(int connfd);
void closeChannel(int connfd);
void connectionTune(int connfd);
void connectionOpen(int connfd);
void connectionStart(int connfd);
void closeConnection(int connfd);