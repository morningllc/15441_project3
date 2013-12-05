#include "dns_parser.h"
#include "dns.h"
#include "dns_robin.h"
#include "dns_dijkstra.h"
#include "dns_log.h"

extern status_t *DNS_stat;
extern int verbal;

void checkbuf(char *buf,int len){
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
	// if(packet->header.qr==1){
		send_packet_t *sendPacket;

		// int tt = len-HEADER_LEN;
		// fprintf(stdout, "id: %d\n",packet->header.id);
		// fprintf(stdout, "qr: %d\n",packet->header.qr);
		// fprintf(stdout, "opcode: %d\n",packet->header.opcode);
		// fprintf(stdout, "aa: %d\n",packet->header.aa);
		// fprintf(stdout, "tc: %d\n",packet->header.tc);
		// fprintf(stdout, "rd: %d\n",packet->header.rd);
		// fprintf(stdout, "ra: %d\n",packet->header.ra);
		// fprintf(stdout, "z: %d\n",packet->header.z);
		// fprintf(stdout, "qdcount: %d\n",packet->header.qdcount);
		// fprintf(stdout, "ancount: %d\n",packet->header.ancount);
		// fprintf(stdout, "nscount: %d\n",packet->header.nscount);
		// fprintf(stdout, "arcount: %d\n",packet->header.arcount);
		// fprintf(stdout, "name: %s\n",packet->data);
		// fprintf(stdout, "qtype: %d\n",(*(unsigned short *)(packet->data+strlen(packet->data)+0)));
		// fprintf(stdout, "qclass: %d\n",(*(unsigned short *)(packet->data+strlen(packet->data)+2)));
		// fprintf(stdout, "tt: %d l-h: %d\n",tt,(int)strlen(packet->data)+3);
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

	answer_t answer;
	answer.name=0x0cc0;
  	answer.type=1;
  	answer._class=1;
	answer.ttl=0;
	answer.ttl2=0;
  	answer.rdlength=4;
  	answer.ip1=ip->ip[0];
  	answer.ip2=ip->ip[1];
  	answer.ip3=ip->ip[2];
  	answer.ip4=ip->ip[3];

    // sprintf(ret+len,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
    // 	0xc,0xc0,1,0,1,0,0,0,0,0,4,0,ip->ip[0],ip->ip[1],ip->ip[2],ip->ip[3]);
    memcpy(ret+len,&answer,sizeof(answer_t));

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


