#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dns_packet.h"

int construct_request_packet(const char *node, char *buf, int *len)
{
	int header_len = sizeof(header_t);
	header_t *header = calloc(1,sizeof(header_t));
	header->qdcount = 1;
	memcpy(buf, header, header_len);

	int question_len = 22;
	sprintf(buf+header_len,"%c%s%c%s%c%s%c%s%c%hu%hu",5,"video",2,"cs",3,"cmu",3,"edu",0,1,1);

	*len = header_len+question_len;
	return 0;
}

int parse_response(char *buf, char *ip)
{
	char *answer_buf = buf+sizeof(header_t)+22;
	answer_t *answer = calloc(1,sizeof(answer_t));
	memcpy(answer, answer_buf, sizeof(answer_t));
	sprintf(ip,"%hu.%hu.%hu.%hu",answer->ip1,answer->ip2,answer->ip3,answer->ip4);
	return 0;
}
