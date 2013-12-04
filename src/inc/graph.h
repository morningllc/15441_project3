#ifndef _GRAPH_
#define _GRAPH_

#include "dns.h"

typedef struct gnode
{
	int seqNum;
	char name[MAXLINE];
	struct gnode *neighbors;
	struct gnode *next;
}gnode_t;

gnode_t *construct_graph(char *file);

gnode_t *update_graph(gnode_t *graph, char *name, int seqNum, char *neighbors);

gnode_t *construct_neighbors(char *neighbors);

void free_neighbors(gnode_t *neighbors);

void free_graph(gnode_t *graph);

#endif
