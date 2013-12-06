#include "dns_parser.h"
#include "dns.h"
#include "dns_robin.h"
#include "dns_dijkstra.h"
#include "dns_log.h"

extern status_t *DNS_stat;
extern int verbal;

/**
 * this function used for debug
 */
void checkbuf(char *buf,int len){
	if(verbal<=2) return;
	fprintf(stdout, "\n***********len:%d - %p**************\n",len,buf);
	for (int i=0;i<len;i++){
		
		if(i%2 == 0) 
			fprintf(stdout, " ");
		if(i%16 == 0)
			fprintf(stdout, "\n");

		fprintf(stdout, "%2x",*(uint8_t *)(buf+i));
		
	}
	fprintf(stdout, "\n*************************\n\n");
}

/**
 * parse dns request
 */
int parse(char *buf,size_t len,SA *addr){
	if(verbal>1)
		fprintf(stdout, "--------------in parse-------------\n");
	checkbuf(buf,len);
	struct sockaddr_in tmpaddr =  *((struct sockaddr_in*)addr);
 	char* clientIP = inet_ntoa(tmpaddr.sin_addr);

	char name[MAXLINE] = {0};
	char data[MAXLINE] = {0};

	packet_t *packet = (packet_t *) buf;

	unsigned short requestID = packet->header.id;
	send_packet_t *sendPacket;
	parseRequestData(packet->data,name);
	fprintf(stdout,"name: %s\n",name);
	if(!strcmp(name,REQUESTNAME)){
		int datalen=buildResponseData(packet->data,len-HEADER_LEN,data,clientIP);
   		sendPacket=construct_response_packet(requestID,0,datalen,data,addr);
   		checkbuf((char *)(sendPacket->data),datalen+HEADER_LEN);
	}else{
		logWrite(clientIP,name,"0.0.0.0");
		sendPacket=construct_response_packet(requestID,3,len-HEADER_LEN,packet->data,addr);
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

/**
 * parse the data part of dns request
 */
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

/**
 * build the data part of response
 */
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

	answer_t answer;
	answer.name=0x0cc0;
  	answer.type=0x100;
  	answer._class=0x100;
	answer.ttl=0;
	answer.ttl2=0;
  	answer.rdlength=0x400;
  	answer.ip1=ip->ip[0];
  	answer.ip2=ip->ip[1];
  	answer.ip3=ip->ip[2];
  	answer.ip4=ip->ip[3];

    // sprintf(ret+len,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
    // 	0xc,0xc0,1,0,1,0,0,0,0,0,4,0,ip->ip[0],ip->ip[1],ip->ip[2],ip->ip[3]);
    memcpy(ret+len,&answer,sizeof(answer_t));
	return len+16;
}

/**
 * get ip address of server using 
 * round robin or LSAs method
 */
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


