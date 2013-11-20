#ifndef _PROXY_H_
#define _PROXY_H_

#include <netinet/in.h>
#include <netinet/ip.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/mman.h>
#include <sys/types.h>      
#include <fcntl.h>



 
#define MAXLINE  8192
#define BUFFERSIZE 8192
#define LISTENQ  1024

typedef struct sockaddr SA;

typedef struct{
	char *ptr;
	char *buf;
	size_t count;
	size_t length;
	size_t size;
} buffer;

typedef struct{
	int fd;	
	char *remote_addr;
	int method;
	int version;
	int close;
	int contentlen;
	char uri[MAXLINE];
	buffer *buf_recv;
	buffer *buf_send;
	
} client_t;

typedef struct {
	int maxfd;
	fd_set read_set;
	fd_set write_set;
	fd_set ready_read;
	fd_set ready_write;
	
	int nready;
	int maxi;
	client_t clients[FD_SETSIZE];
} pool;



void initPool(pool *p);

/**
 * add two listen fds to pool
 */
void setPool(int httpfd,pool *p);

void addClient(int connfd,pool *p,struct sockaddr_in clientaddr);

void checkClients(pool *p);
int open_listenfd(int port) ;


#endif