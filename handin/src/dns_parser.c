#include "dns_parser.h"
#include "dns.h"
#include "dns_robin.h"
#include "dns_dijkstra.h"
#include "dns_log.h"

extern status_t *DNS_stat;
extern int verbal;

int parse(char *buf,size_t len,SA *addr){
	if(verbal>1)
		fprintf(stdout, "--------------in parse-------------\n");
	
	struct sockaddr_in tmpaddr =  *((struct sockaddr_in*)addr);
 	char* clientIP = inet_ntoa(tmpaddr.sin_addr);

	char name[MAXLINE] = {0};
	char data[MAXLINE] = {0};

	packet_t *packet = (packet_t *) buf;

	// if(packet->header.qr==1){
		send_packet_t *sendPacket;
		parseRequestData(packet->data,name);
		fprintf(stderr,"name: %s\n",name);
		if(!strcmp(name,REQUESTNAME)){
			int datalen=buildResponseData(packet->data,len-HEADER_LEN,data,clientIP);
	   		sendPacket=construct_response_packet(0,datalen,data,addr);
		}else{
			logWrite(clientIP,name,"");
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
	// }
	// if(verbal>1)
	// 	fprintf(stderr, "request is not a query\n");
	// return -1;

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

int buildResponseData(char *request,size_t len, char* ret,char *clientIP){
	if(verbal>1)
		fprintf(stdout, "-----------in buildResponseData----------\n");	
	memcpy(ret,request,len);

	

	ip_t *ip=getIP(clientIP);


	if(ip==NULL){
		logWrite(clientIP,REQUESTNAME,"");
	 	return len;
	}

	logWrite(clientIP,REQUESTNAME,ip->ip_str);

    sprintf(ret+len,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
    	0xc,0xc0,1,0,1,0,0,0,0,0,4,0,ip->ip[0],ip->ip[1],ip->ip[2],ip->ip[3]);

	return len+16;
}

ip_t *getIP(char *clientIP){

	if(verbal>1)
		fprintf(stdout, "-----in getIP-----\n");

	if(DNS_stat->robinFlag){
		return getIP_Robin(DNS_stat->robin_list);
	}else{
		return getIP_LSAs(DNS_stat->lsaGraph, clientIP, DNS_stat->robin_list);
	}
	if(verbal>1)
			fprintf(stdout, "-----in getIP fail-----\n");
	return NULL;
}


