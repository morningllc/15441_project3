#include "dns_parser.h"
#include "dns.h"

extern status_t *DNS_stat;
extern int verbal;

int parse(char *buf,size_t len,SA *addr){
	if(verbal>1)
		fprintf(stdout, "--------------in parse-------------\n");
	
	char name[MAXLINE] = {0};
	char data[MAXLINE] = {0};

	packet_t *packet = (packet_t *) buf;

	if(packet->header.qr==1){
		send_packet_t *sendPacket;
		parseRequestData(packet->data,name);

		if(!strcmp(name,REQUESTNAME)){
			buildResponseData(packet->data,data);
			sendPacket=construct_response_packet(0,strlen(data),data,addr);
		}else{
			sendPacket=construct_response_packet(3,strlen(packet->data),packet->data,addr);
		}

		if(sendPacket!=NULL){
			enqueue(DNS_stat->send_packets,sendPacket);
		}

		if(verbal>1)
		fprintf(stdout, "--------------in parse - done-------------\n");
		return 0;
	}
	if(verbal>1)
		fprintf(stderr, "request is not a query\n");
	return -1;

}

int parseRequestData(char *data,char *buf){
	if(verbal>1)
		fprintf(stdout, "-----------in parseRequestData----------\n");
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

int buildResponseData(char *request, char* ret){
	if(verbal>1)
		fprintf(stdout, "-----------in buildResponseData----------\n");	

	size_t len = strlen(request);
	strncpy(ret,request,len);
	// char name[MAXLINE];
	// sprintf(name,"%c%s%c%s%c%s%c%s%c",5,"video",2,"cs",3,"cmu",3,"edu",0);
	unsigned short name = 0xc0c;
	sprintf(ret+len,"%hu%hu%hu%hu%hu%c%c%c%c",name,1,1,0,4,1,0,0,1);
	return 0;
}


