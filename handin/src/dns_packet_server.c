#include "dns.h"
#include "dns_packet_server.h"
#include "dns_parser.h"

extern status_t *DNS_stat;
extern int verbal;

/**
 * construct a packet waiting for sent
 */
send_packet_t* construct_response_packet(unsigned short id,uint8_t rcode,int len,char *data,SA *addr){
	if(verbal>1)
		fprintf(stdout, "--------------in construct_response_packet-------------\n");

	send_packet_t *packet;
	if((packet=(send_packet_t *)malloc(sizeof(send_packet_t)))==NULL){
		return NULL;
	}
	packet->size=len+HEADER_LEN;
	packet->sent=0;
	packet->address=*addr;

	if((packet->data=(packet_t *)malloc(sizeof(packet_t)))==NULL){
		free(packet);
		return NULL;
	}

	header_t *header=&(packet->data->header);

	header->id=id;
	*(((char *)header)+2) = 0x84;
	*(((char *)header)+3) = 0x0;
	// header->qr=1;
	// header->opcode=0;
	// header->aa=1;
	// header->tc=0;
	// header->rd=0;
	// header->ra=0;
	// header->z=0;
	// header->rcode=rcode;
	header->qdcount=0x0100;
	header->ancount=0x0100;
	header->nscount=0;
	header->arcount=0;

	if(data != NULL)
		memcpy(packet->data->data,data,(size_t)len);

	return packet;

}

/**
 * free packet
 */
void free_send_packet(send_packet_t *p){
	free(p->data);
	free(p);
}











