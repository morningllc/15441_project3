#ifndef _DNS_PACKET_
#define _DNS_PACKET_

typedef struct header
{
	unsigned int id:16;
	unsigned int qr:1;
	unsigned int opcode:4;
	unsigned int aa:1;
	unsigned int tc:1;
	unsigned int rd:1;
	unsigned int ra:1;
	unsigned int z:3;
	unsigned int rcode:4;
	unsigned int qdcount:16;
	unsigned int ancount:16;
	unsigned int nscount:16;
	unsigned int arcount:16;
}header_t;

typedef struct answer
{
	unsigned int name:16;
	unsigned int type:16;
	unsigned int _class:16;
	unsigned int ttl:32;
	unsigned int rdlength:16;
	unsigned int ip1:8;
	unsigned int ip2:8;
	unsigned int ip3:8;
	unsigned int ip4:8;
}answer_t;



#endif
