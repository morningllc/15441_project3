#include "mydns.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <proxy.h>
#include "dns_packet.h"

extern status_t *proxy_stat;

int init_mydns(const char *dns_ip, unsigned int dns_port)
{
	return 0;
}

int resolve(const char *node, const char *service,
						const struct addrinfo *hints, struct addrinfo **res)
{
	int sock;
	struct sockaddr_in addr,dnsaddr;

	int fdmax;
	fd_set read_fd;

	FD_ZERO(&read_fd);

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(proxy_stat->fakeIP);
	addr.sin_port = htons(0);

	dnsaddr.sin_family = AF_INET;
	dnsaddr.sin_addr.s_addr = inet_addr(proxy_stat->dnsIP);
	dnsaddr.sin_port = htons(proxy_stat->dnsPort);

	if((sock = socket(AF_INET, SOCK_DGRAM, 0)) == -1){
		printf("Failed creating dns socket\n");
		exit(-1);
	}

	if(bind(sock, (struct sockaddr *)&addr, sizeof(addr))){
		printf("Failed binding dns socket\n");
		exit(-1);
	}

	char buf[MAXLINE] = {0};
	int len = 0;
	construct_request_packet(node, buf, &len);

	printf("send to dns server\n");

	sendto(sock, buf, len, 0, (struct sockaddr *)&dnsaddr, sizeof(dnsaddr));

	fdmax = sock;
	FD_SET(sock, &read_fd);

	if(select(fdmax+1, &read_fd, NULL, NULL, NULL) < 0){
		fprintf(stderr, "select error\n");
		exit(-1);
	}
  
	printf("-------------dns select done----------------\n ");

	struct sockaddr_in from;
	socklen_t fromlen = sizeof(from);

	char buf2[MAXLINE] = {0};
	char ip[MAXLINE] = {0};

	int num = recvfrom(sock, buf2, MAXLINE, 0, (struct sockaddr *)&from, &fromlen);

	printf("recv %d bytes\n", num);

	parse_dns_response(buf2, ip);
	
	printf("ip = %s\n", ip);

	if(getaddrinfo(ip,service,NULL,res) != 0){
		fprintf(stderr,"dns getaddrinfo error\n");
		exit(-1);
	}

	return 0;
}
