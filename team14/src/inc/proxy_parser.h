#ifndef _PROXY_PASER_H_
#define _PROXY_PASER_H_

#include "proxy.h"


#define METHOD_GET      1
#define METHOD_HEAD     2
#define METHOD_POST     3
#define METHOD_BAD      -1

#define HTTP_1_1        1
#define HTTP_1_0        2
#define HTTP_BAD        -1 

#define TYPE_MANIFEST   1
#define TYPE_VIDEO      2
#define TYPE_OTHER      3


void checkBuffer(buffer *b,char *name);
int parseClientRequest(socket_t *pair);
int parseMethod(socket_t *pair,char *buf);

int parseLine(socket_t *pair, char *buf);
int parseServerHeader(socket_t *pair);
int parseManifestFile(char *buf);
#endif
