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
#define BUFFERSIZE 1024
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

typedef struct{

	int client_fd;
	int server_fd;	
	char *remote_addr;

	char host[MAXLINE];
	int host_port;

	int method;
	int version;
	int request_type;
	char path[MAXLINE];

	char *urn;
	int request_rate;
	int seg_num;
	int frag_num;

	int close;
	int contentlen;
	
	buffer *buf_server;
	buffer *buf_client;
	buffer *buf_send;
	
} socket_t;

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

typedef struct {	
	float alpha;
	int listenPort;
	int dnsPort;
	char *logFile;
	char *fakeIP;
	char *dnsIP;
	char *wwwIP;
	pool *p;
}status_t;

status_t* initProxy(int argc, char **argv);

void initPool(pool *p);

void setPool(int httpfd,pool *p);

void addClient(int connfd,pool *p,struct sockaddr_in clientaddr);

void checkClients(pool *p);

int open_listenfd(int port) ;


#endif
