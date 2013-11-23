#ifndef _PROXY_PROCESS_H_
#define _PROXY_PROCESS_H_

#include "proxy.h"

int open_serverfd(socket_t *pair);


int buildRequestContent(socket_t *pair);
int buildManifestContent(socket_t *pair,char *header);
int buildRequestHeader(socket_t *pair, char* header);
int addData(buffer *b,void* data,size_t len);

#endif