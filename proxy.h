#ifndef _PROXY_H_
#define _PROXY_H_

#include <netinet/in.h>
#include <netinet/ip.h>
#include <netdb.h>
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
#include <time.h>


#include "proxy_queue.h"
 
#define MAXLINE  1024
#define BUFFERSIZE 8192
#define LISTENQ  1024

#define USAGE "usage: proxy <log> <alpha> <listen-port> <fake-ip> <dns-ip> <dns-port> [<www-ip>]\n"
typedef struct sockaddr SA;

typedef struct{
	char *ptr;
	char *buf;
	size_t count;
	size_t length;
	size_t size;
} buffer;

typedef enum {HEADER, CONTENT} server_t;

typedef struct{

	int client_fd;
	int server_fd;	
	char *remote_addr;

	char host[MAXLINE];
	int host_port;

	int method;
	char version[MAXLINE];
	int request_type;
	char path[MAXLINE];

	char *urn;
	int request_rate;
	int seg_num;
	int frag_num;

	int close;
	
	int recv;
	int left;
	int contentlen;
	char *content_buf;
	
	server_t server_stat;

	queue_t *requestQueue;
	buffer *buf_server;
	buffer *buf_client;
	buffer *buf_send_client;
	buffer *buf_send_server;
	
} socket_t;

typedef struct {
	int maxfd;
	fd_set read_set;
	fd_set write_set;
	fd_set ready_read;
	fd_set ready_write;
	
	int nready;
	int maxi;
	socket_t pairs[FD_SETSIZE];
} pool;

typedef struct {	
	float alpha;
	int listenPort;
	int dnsPort;
	char *logFile;
	char *fakeIP;
	char *dnsIP;
	char *wwwIP;
	pool *p;

	int bitrates[4];
	int bitrate;
	float t;
	
}status_t;

typedef struct {
	int request_type;
	time_t send_time;
}pstate_t;


status_t* initProxy(int argc, char **argv);
void initPool(pool *p);
void initSocketPair(socket_t *pair);
void setPool(int httpfd,pool *p);

void addSocketPair(int connfd,pool *p,struct sockaddr_in clientaddr);
void checkSocketPairs(pool *p);

void doIt_ReadClient(socket_t *pair);
void doIt_ReadServer(socket_t *pair);
void doIt_Process(socket_t *pair);
void doIt_SendToServer(socket_t *pair);
void doIt_SendToClient(socket_t *pair);
int open_listenfd(int port);

#endif
