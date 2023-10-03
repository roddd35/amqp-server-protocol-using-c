#pragma once

#define _GNU_SOURCE
#define MAX_CHAR 999

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

int readHeader(int connfd);
void connectionTune(int connfd);
void connectionStart(int connfd);