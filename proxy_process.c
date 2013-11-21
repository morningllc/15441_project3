#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "proxy.h"
#include "proxy_socket.h"

int open_serverfd(socket_t *pair)
{
  char *server_ip = proxy_stat->wwwIP;
  char server_port[] = "8080";
  char *fake_ip = proxy_stat->fakeIP;
	char rand_port[] = "5555";

	int status, sock;
  struct addrinfo hints;
  memset(&hints, 0, sizeof(struct addrinfo));
  struct addrinfo *servinfo, *addr;
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_PASSIVE;

  if((status = getaddrinfo(server_ip, server_port, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo error\n");
		return -1;
	}

  if((status = getaddrinfo(fake_ip, rand_port, NULL, &addr)) != 0){
		fprintf(stderr, "getaddrinfo error\n");
    return -1;
	}

	if((sock = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		fprintf(stderr, "Socket failed\n");
		return -1;
	}

	if(bind(sock, (struct sockaddr *)addr, sizeof(*addr))){
		fprintf(stderr, "Failed binding socket\n");
		return -1;
	}

  if(connect(sock, servinfo->ai_addr, servinfo->ai_addrlen)){
		fprintf(stderr, "Connect failed\n");
		return -1;
	}

	freeaddrinfo(servinfo);
	freeaddrinfo(addr);

  pair->server_fd = sock;

	return 1;
}
