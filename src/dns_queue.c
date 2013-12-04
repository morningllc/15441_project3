#include "dns_queue.h"
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
void enqueue(queue_t *queue, void *data)
{
  assert(queue != NULL);
  assert(data != NULL);

  node_t *node = malloc(sizeof(node_t));
  node->data = data;
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
void *dequeue(queue_t *queue)
{
  assert(queue != NULL);
  //fprintf(stdout, "-----deQ-----\n");
  node_t *head = queue->head;
  if(queue->size == 0){
  assert(queue->head == NULL);
  assert(queue->tail == NULL);
    return NULL;    
  }
  else{
  void *data = head->data;
  node_t *next = head->next;
    if(next != NULL){
    next->prev = NULL;
    queue->head = next;
  } 
  else{
    queue->head = queue->tail = NULL;
  }
  free(head);
  queue->size--;
  return data;
  } 
}

void free_queue(queue_t *queue)
{
  while(queue->size != 0){
    dequeue(queue);
  }
  free(queue);
}

int enqueuePQ(queue_t *queue, void *data,int weight)
{
  assert(queue != NULL);
  assert(data != NULL);

  node_t *node = malloc(sizeof(node_t));
  node->weight=weight;
  node->data = data;
  node->prev = NULL;
  node->next = NULL;

  if(queue->tail == NULL){
    queue->head = node;
    queue->tail = node;
  }else{
    node_t *ptr;
    for(ptr = queue->head;ptr != NULL; ptr = ptr->next){
      if(weight <= ptr->weight){
        if(ptr == queue->head){
          node->next = queue->head;
          queue->head = node;
        }else{
          node->prev = ptr->prev;
          ptr->prev->next = node;
          ptr->prev = node;
          node->next = ptr;
        }
        break;
      }
    }
    if(ptr == NULL){
      queue->tail->next = node;
      node->prev = queue->tail;
      queue->tail = node;
    }
  }

  queue->size++;
  return 0;
}