#ifndef _DSN_DIJ_H_
#define _DSN_DIJ_H_

#include "dns.h"
#include "graph.h"


typedef struct d_node_s{
	char name[MAXLINE];
	int weight;
	struct d_node_s *next;
}d_node_t;


ip_t *getIP_LSAs(gnode_t* graph, char* src, robin_list_t* servers);
int containNode(char* name, d_node_t *list);
gnode_t *findGraphNode(char *name,gnode_t* graph);
ip_t *isServer(char *name,robin_list_t* list);
void addToPQ(gnode_t *neighbors,int weight,queue_t *PQ);
void freeALL(queue_t *PQ,d_node_t *list);

#endif