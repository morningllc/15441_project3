#include "dns_parser.h"
#include "dns.h"
#include "dns_robin.h"

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
			printf("right\n");
			buildResponseData(packet->data,len-HEADER_LEN,data,addr);
	   	sendPacket=construct_response_packet(0,len-HEADER_LEN+16,data,addr);
		}else{
			sendPacket=construct_response_packet(3,len-HEADER_LEN,packet->data,addr);
		}

		if(sendPacket!=NULL){
			char *tmp = sendPacket->data->data;
			tmp+=len-HEADER_LEN;
			fprintf(stdout,"%d.%d.%d.%d\n",(int)(*(tmp+12)),(int)(*(tmp+13)),(int)(*(tmp+14)),(int)(*(tmp+15)));
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

int buildResponseData(char *request,size_t len, char* ret,SA *addr){
	if(verbal>1)
		fprintf(stdout, "-----------in buildResponseData----------\n");	

	//size_t len = strlen(request);
	memcpy(ret,request,len);
	// char name[MAXLINE];
	// sprintf(name,"%c%s%c%s%c%s%c%s%c",5,"video",2,"cs",3,"cmu",3,"edu",0);
	//unsigned short name = 0xc00c;
	ip_t *ip=getIP(addr);
    sprintf(ret+len,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
    	0xc,0xc0,1,0,1,0,0,0,0,0,4,0,ip->ip[0],ip->ip[1],ip->ip[2],ip->ip[3]);

return 0;
}

ip_t *getIP(SA *addr){

	// (struct sockaddr_in*)addr;

	if(DNS_stat->robinFlag){
		return getIP_Robin(DNS_stat->robin_list);
	}else{
		// getIP_LSAs(DNS_stat->graph, char* src, servers);
	}

	return NULL;
}


