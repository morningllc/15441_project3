#include "dns_parser.h"
#include "dns.h"

extern status_t *DNS_stat;
extern int verbal;

int parse(char *buf,size_t len,SA *addr){
	char name[MAXLINE];
	packet_t *packet = (packet_t *) buf;
	if(packet->header.qr==1){
		parseData(packet->data,name);
		if(!strcmp(name,REQUESTNAME)){
			// construct_response_packet(packet,len,addr);
		}
	}
	return -1;

}
int parseData(char *data,char *buf){
	int len=0;
	char *ptr = data;
	int curr;
	while((curr=(int)*ptr)!=0){
		ptr++;
		strncpy(buf+len,ptr,curr);
		len+=curr;
		ptr+=curr;
		strncpy(buf+len,".",1);
		len++;
	}
	*(buf+len-1)=0;
	return 0;
}

