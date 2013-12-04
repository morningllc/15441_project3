#ifndef _DNS_ROBIN_H_
#define _DNS_ROBIN_H_

typedef struct {
	char ip_str[20];
	uint8_t ip[4];
} ip_t;

typedef struct robin_node_s{
	ip_t ip;
	struct robin_node_s *next;
}robin_node_t;

typedef struct{
	int size;
	robin_node_t *head;
	robin_node_t *tail;
	robin_node_t *ptr;
}robin_list_t;


robin_list_t* initRobinList(char *filename);
void closeFile(FILE *f);
ip_t *getIP_Robin(robin_list_t* list);
#endif