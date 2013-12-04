#ifndef _DNS_H_
#define _DNS_H_

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


#include "dns_queue.h"
#include "dns_robin.h"

#define MAXLINE  1024
#define BUFFERSIZE 8192
#define LISTENQ  1024

#define USAGE "usage: nameserver [-r] <log> <ip> <port> <servers> <LSAs>\n"
typedef struct sockaddr SA;

#include "dns_packet_server.h"

typedef struct {
	int robinFlag;	
	int port;
	char *ip;
	char *logFile;
	char *serverFile;
	char *LSAs;
	queue_t *send_packets;
	send_packet_t *sending;
	robin_list_t* robin_list;
}status_t;


status_t* initDNSServer(int argc, char **argv);
int doIt_Read(int fd);
int doIt_Send(int fd);
int open_listenfd(char* ip, int port);
void closeDNSServer();

#endif