#include "dns.h"
#include "dns_robin.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
robin_list_t* initRobinList(char *filename){
	FILE *file;
	char line[MAXLINE];
	
	robin_list_t *list = (robin_list_t *)malloc(sizeof(robin_list_t));
	if(list==NULL){
		fprintf(stderr,"error while creating list:");
		return NULL;
	}
	list->size=0;
	list->head=NULL;
	list->tail=NULL;
	list->ptr=NULL;

	if((file=fopen(filename,"r"))==NULL){
		free(list);
		fprintf(stderr, "error in open file :%s\n",filename);
		return NULL;
	}

	while(fgets(line,MAXLINE,file)!=NULL){
		robin_node_t *new = (robin_node_t *)malloc(sizeof(robin_node_t));
		new->next=NULL;
		sscanf(line,"%"SCNu8".%"SCNu8".%"SCNu8".%"SCNu8"",
			&new->ip.ip[0],&new->ip.ip[1],&new->ip.ip[2],&new->ip.ip[3]);
		strcpy(new->ip.ip_str,line);
		int tmplen = strlen(new->ip.ip_str);
		new->ip.ip_str[tmplen-1]=0;
		if(list->head==NULL){
			list->head=new;
			list->tail=new;
		}else{
			list->tail->next=new;
			list->tail=new;
		}
		list->size++;
	}
	list->ptr=list->head;
	closeFile(file);
	return list;
}

void closeFile(FILE *f){
	if(f!=NULL){
		fclose(f);
		f=NULL;
	}
}

ip_t *getIP_Robin(robin_list_t* list){
	if (list->size==0) return NULL;

	ip_t *ret = &list->ptr->ip;
	if(list->ptr == list->tail) 
		list->ptr = list->head;
	else list->ptr = list->ptr->next;

	return ret;
}