#ifndef _PROXY_SOCKET_H_
#define _PROXY_SOCKET_H_

#include "proxy.h"

#define MAX_REQLENGTH 8192
buffer *initBuffer();
void freeBuffer(buffer *b);
int resetBuffer(buffer *b);

int initSocketBuffer(socket_t *s);
void cleanSocketBuffer(socket_t *s);
int resetSocketBuffer(socket_t *s);

void initSocketPair(socket_t *pair);
void resetSocketPair(socket_t *s);

int clientReadByte(socket_t *pair);
ssize_t readLine(buffer* src,char *usrbuf, ssize_t maxlen);

#endif