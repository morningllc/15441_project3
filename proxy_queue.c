#include "proxy_queue.h"
#include <assert.h>

/**
 * build an empty queue
 */
queue_t *new_queue()
{
    queue_t *q = malloc(sizeof(queue_t));
    q->size = 0;
    q->head = q->tail = NULL;
    return q;
}

/**
 * add new data into queue
 */
void enqueue(queue_t *queue, int type)
{
    assert(queue != NULL);
    fprintf(stdout, "****************enqueue : %d*****************\n",type );
    node_t *node = malloc(sizeof(node_t));
    node->data = type;
    node->prev = NULL;
    node->next = NULL;

    if(queue->tail != NULL){
      	queue->tail->next = node;
      	node->prev = queue->tail;
      	queue->tail = node;
    }
    else{
    	  queue->head = queue->tail = node;
    }

    queue->size++;
    return;
}

/**
 * pop out an element
 */
int dequeue(queue_t *queue)
{

    assert(queue != NULL);
    assert(queue->size!=0);
    node_t *head = queue->head;
 
  	int ret = head->data;
  	node_t *next = head->next;
    if(next != NULL){
    	  next->prev = NULL;
    	  queue->head = next;
  	}else{
  	    queue->head = queue->tail = NULL;
  	}
  	free(head);
  	queue->size--;
    fprintf(stdout, "****************dequeue : %d*****************\n",ret );
  	return ret;
}

void free_queue(queue_t *queue)
{
  while(queue->size != 0)
    dequeue(queue);
  free(queue);
}
