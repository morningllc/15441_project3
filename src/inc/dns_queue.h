#ifndef _DNS_QUEUE_H_
#define _DNS_QUEUE_H_

#include <stdlib.h>
#include <malloc.h>

typedef struct node
{
  void *data;
  struct node *prev;
  struct node *next;
}node_t;

typedef struct queue
{
	int size;
	node_t *head;
	node_t *tail;
}queue_t;

queue_t *new_queue(void);

void enqueue(queue_t *queue, void *data);

void *dequeue(queue_t *queue);

void free_queue(queue_t *queue);

#endif
