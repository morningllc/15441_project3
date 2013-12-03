#include "dns_packet_server.h"
#include "dns.h"
#include "dns_parser.h"
extern status_t *DNS_stat;
extern int verbal;

send_packet_t* construct_response_packet(uint8_t rcode,int len,char *data,SA *addr){
	
	send_packet_t *packet;
	if((packet=(send_packet_t *)malloc(sizeof(send_packet_t)))==NULL){
		return NULL;
	}
	packet->size=len+HEADER_LEN;
	packet->sent=0;
	packet->address=addr;

	if((packet->data=(packet_t *)malloc(sizeof(packet_t)))==NULL){
		free(packet);
		return NULL;
	}

	header_t *header=&(packet->data->header);

	header->id=0;
	header->qr=0;
	header->opcode=0;
	header->aa=1;
	header->tc=0;
	header->rd=0;
	header->ra=0;
	header->z=0;
	header->rcode=rcode;
	header->qdcount=0;
	header->ancount=0;
	header->nscount=0;
	header->arcount=0;

	if(data != NULL)
		memcpy(packet->data->data,data,(size_t)len);

	return packet;

}
