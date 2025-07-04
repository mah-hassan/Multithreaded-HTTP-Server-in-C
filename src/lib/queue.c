#include <stdlib.h>
#include "queue.h"

queue *create_queue() {
    queue *q = malloc(sizeof(queue));
    if (q == NULL) {
        return NULL; // Memory allocation failed
    }
    q->head = NULL;
    q->tail = NULL;
    return q;
}

void enqueue(queue *q, int *data) {
    node *new_node = malloc(sizeof(node));
    if(new_node == NULL)
    {
        return; 
    }
    new_node->data = data; 
    new_node->next = NULL;
    if(q->tail != NULL)
    {
        q->tail->next = new_node;
    }

    q->tail = new_node; 

    if(q->head == NULL)
    {
        q->head = new_node;
    }
}

int *dequeue(queue *q) {
    if(q->head == NULL) {
        return NULL; 
    }
    
    node *temp = q->head;
    int *result = temp->data;

    q->head = q->head->next;
    
    if(q->head == NULL) {
        q->tail = NULL; 
    }
    
    free(temp); 
    return result; 
}