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
