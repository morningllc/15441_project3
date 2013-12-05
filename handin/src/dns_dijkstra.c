#include "dns_dijkstra.h"
#include "dns.h"
#include "dns_parser.h"

extern int verbal;

void checkGraph(gnode_t* graph){
	fprintf(stdout, "\n**********checkGraph**********\n");
	gnode_t *ptr=graph;
	while(ptr !=NULL){
		fprintf(stdout, "%s --> ",ptr->name);
		gnode_t *ptr2=ptr->neighbors;
		while (ptr2 != NULL){
			fprintf(stdout, "- %s -",ptr2->name);
			ptr2=ptr2->next;
		}
		fprintf(stdout, " \n");
		ptr = ptr->next;
	}
	fprintf(stdout, "*********************************\n\n");
}

void checkQ(queue_t *Q){
	fprintf(stdout, "\n**********checkQ : size = %d head:%p**********\n",Q->size,Q->head);
	node_t* ptr = Q->head;
	while(ptr!=NULL){
		d_node_t * data = (d_node_t *)ptr->data;
		fprintf(stdout, "%s  --  %d -- %p -- %p\n",data->name,data->weight,data,data->next);
		ptr=ptr->next;
	}
	fprintf(stdout, "*********************************\n\n");

}

void checkL(d_node_t *L){
	fprintf(stdout, "\n**********checkL**********\n");
	d_node_t* ptr = L;
	while(ptr!=NULL){
		
		fprintf(stdout, "%s  --  %d -- %p -- %p\n",ptr->name,ptr->weight,ptr,ptr->next);
		ptr=ptr->next;
	}
	fprintf(stdout, "*********************************\n\n");

}


ip_t *getIP_LSAs(gnode_t* graph, char* src, robin_list_t* servers){
	if(verbal>2) checkGraph(graph);
	if(verbal>1)
		fprintf(stdout, "-----in getIP_LSAs-----\n");
	if(graph==NULL||(servers->size == 0)) return NULL;
	ip_t *ret=NULL;

	d_node_t *visit=NULL;
	queue_t *PQ = new_queue();

	d_node_t* curr = (d_node_t *)malloc(sizeof(d_node_t));
	strcpy(curr->name,src);
	curr->weight = 0;
	curr->next = NULL;
	
	do{
		if(verbal>2)
			checkQ(PQ);
		if(containNode(curr->name,visit)) {
			free(curr);
			curr = NULL;
			curr=(d_node_t *)dequeue(PQ);
			continue;
		}
		curr->next = visit;
		visit = curr;

		if((ret = isServer(curr->name,servers))!=NULL) {
			break;
		}		

		gnode_t *tmp = findGraphNode(curr->name,graph);
		if(tmp == NULL) {
			curr=(d_node_t *)dequeue(PQ);
			continue;
		}

		addToPQ(tmp->neighbors,curr->weight,PQ);

		curr=(d_node_t *)dequeue(PQ);

	}while(curr!=NULL);

	freeALL(PQ,visit);


	if(verbal>2)
		fprintf(stdout, "-----getIP_LSAs done %p-----\n",ret);
	return ret;
}

int containNode(char* name, d_node_t *list){
	if(verbal>2)
		fprintf(stdout, "-----in containNode : %s-----\n",name);
	if(list==NULL) return 0;
	d_node_t *ptr=list;

	while(ptr!=NULL){
		if(!strcmp(ptr->name,name)){
			if(verbal>2)
				fprintf(stdout, "-----in containNode yes-----\n");
			return 1;
		}
		ptr=ptr->next;
	}
	if(verbal>2)
		fprintf(stdout, "-----in containNode no-----\n");
	return 0;

}

gnode_t *findGraphNode(char *name,gnode_t* graph){
	if(verbal>2)
		fprintf(stdout, "-----in findGraphNode-----\n");
	gnode_t *ptr=graph;
	while(ptr!=NULL){
		if(!strcmp(name,ptr->name)){
			return ptr;
		}
		ptr=ptr->next;
	}
	return NULL;
}

ip_t *isServer(char *name,robin_list_t* list){
	if(verbal>2)
		fprintf(stdout, "-----in isServer: %s-----\n",name);
	robin_node_t *ptr;
	for(ptr=list->head;ptr!=NULL;ptr=ptr->next){
		if(!strcmp(name,ptr->ip.ip_str))
			return &ptr->ip;
	}
	return NULL;
}

void addToPQ(gnode_t *neighbors,int weight,queue_t *PQ){
	if(verbal>2)
		fprintf(stdout, "-----in addToPQ-----\n");
	gnode_t *ptr=neighbors;
	while(ptr!=NULL){
		d_node_t* new = (d_node_t *)malloc(sizeof(d_node_t));
		if(verbal>2)
			fprintf(stdout, "----->new node: %p-----\n",new);
		strcpy(new->name,ptr->name);
		new->weight = weight+1;
		new->next = NULL;
		enqueuePQ(PQ,new,new->weight);
		ptr = ptr->next;
	}

}
void freeALL(queue_t *PQ,d_node_t *list){
	if(verbal>2)
		fprintf(stdout, "-----in freeALL-----\n");
	if(verbal>2) checkQ(PQ);

	while(PQ->size != 0){
		d_node_t *n = dequeue(PQ);
		// fprintf(stdout, "freeQ: %p\n",n);
		free(n);
	}
	free(PQ);
	if(verbal>2)
	    fprintf(stdout, "-----in freeALL -2 -----\n");
	
	if(verbal>2) checkL(list);

	d_node_t *ptr = list;
	while(ptr!=NULL){
		d_node_t *tmp = ptr;
		ptr=ptr->next;
	 	// fprintf(stdout, "freeL: %p\n",tmp);
		free(tmp);
	}

	if(verbal>2)
		fprintf(stdout, "-----freeALL done-----\n");
}