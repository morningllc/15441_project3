#include "dns.h"
#include "graph.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

gnode_t *construct_graph(char *file)
{
	FILE *lsa = fopen(file, "r");

	gnode_t *graph = calloc(1,sizeof(gnode_t));

	char line[MAXLINE] = {0};
	while(fgets(line, MAXLINE, lsa) != NULL){
		char name[MAXLINE] = {0};
		int seqNum = 0;
		char neighbors[MAXLINE] = {0};
		sscanf(line, "%s %d %s", name, &seqNum, neighbors);
		memset(line, 0, MAXLINE);
		graph = update_graph(graph, name, seqNum, neighbors);
	}
/*
	gnode_t *tmp = graph;
	while(tmp != NULL){
		printf("node = %s\n", tmp->name);
		tmp = tmp->next;
	}
*/
	fclose(lsa);
	return graph;
}

gnode_t *update_graph(gnode_t *graph, char *name, int seqNum, char *neighbors)
{
	gnode_t *tmp = graph;
	while(tmp != NULL){
		if(strcmp(tmp->name, name) == 0){
			if(tmp->seqNum >= seqNum){
				return graph;
			}
			else{
				tmp->seqNum = seqNum;
				free_neighbors(tmp->neighbors);
				tmp->neighbors = construct_neighbors(neighbors);
				return graph;
			}
		}	
		tmp = tmp->next;
	}

	gnode_t *new_node = calloc(1,sizeof(gnode_t));
	new_node->seqNum = seqNum;
	strcpy(new_node->name, name);
	new_node->neighbors = construct_neighbors(neighbors);
	new_node->next = graph;
	
	return new_node;
}

gnode_t *construct_neighbors(char *neighbors)
{
	char *search = ",";
	char *token;

	token = strtok(neighbors, search);
	gnode_t *nb = calloc(1,sizeof(gnode_t));
	strcpy(nb->name, token);	

	gnode_t *tmp = nb;
	while((token = strtok(NULL, search)) != NULL){
		gnode_t *next = calloc(1,sizeof(gnode_t));
		strcpy(next->name, token);
		tmp->next = next;
		tmp = next;
	}

	return nb;
}

void free_neighbors(gnode_t *neighbors)
{
	gnode_t *tmp = neighbors;
	gnode_t *next = tmp;
	while(tmp != NULL){
		next = tmp->next;
		free(tmp);
		tmp = next;
	}
	return;
}

void free_graph(gnode_t *graph)
{
	gnode_t *tmp = graph;
	gnode_t *next = tmp;
	while(tmp != NULL){
		next = tmp->next;
		free_neighbors(tmp->neighbors);
		free(tmp);
		tmp = next;
	}
	return;
}
