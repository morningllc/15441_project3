#ifndef _DNS_ROBIN_H_
#define _DNS_ROBIN_H_

typedef struct {
	char ip_str[20];
	uint8_t ip[4];
} ip_t;

typedef struct{
	ip_t ip;
	robin_node_t *next;
}robin_node_t;

typedef struct{
	int size;
	robin_node_t *head;
	robin_node_t *tail;
	robin_node_t *ptr;
}robin_list_t;

#endif