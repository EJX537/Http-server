//-------------------------------------------------------------------------------------------------
// Queue.c
//-------------------------------------------------------------------------------------------------
#include "queue.h"
#include "Handle.h"
#include <unistd.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

pthread_mutex_t Qmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t full = PTHREAD_COND_INITIALIZER;
pthread_cond_t empty = PTHREAD_COND_INITIALIZER;
// Structs ----------------------------------------------------------------------------------------
typedef struct NodeObj* Node;
typedef struct queue {
    void *front;
    void *back;
    int length;
	int size;
} queue_t;
void clear(queue_t *q);
// Objects of the queue
typedef struct NodeObj {
    void *data;
    Node next;
    Node prev;
} NodeObj;
// Create New Node
Node newNode(void *x) {
    Node node;
    node = malloc(sizeof(NodeObj));
    node->data = x;
    node->next = NULL;
    node->prev = NULL;
    return node;
}
// Free Node
void freeNode(Node *pNode) {
    if (pNode != NULL && *pNode != NULL) {   
        free(*pNode);
        *pNode = NULL;
    }
}
// Create a new queue
queue_t *queue_new(int size) {
    queue_t *queue;
    queue = malloc(sizeof(struct queue));
    queue->front = queue->back = NULL;
    queue->size = size;
    queue->length = 0;
    return queue;
}
// Delete queue and free all the nodes
void queue_delete(queue_t **q) {
    if (*q != NULL) {   
        clear(*q);
        pthread_mutex_destroy(lock_mutex(*q));
        pthread_cond_destroy(lock_full(*q));
        pthread_cond_destroy(lock_empty(*q));
        free(*q);
        *q = NULL;
    }
    return;
}
// Append to the end of the queue
bool queue_push(queue_t *q, void *elem) {
    // Lock thread
    pthread_mutex_lock(&Qmutex);
    // Wait if there is the queue is full
    while (q->length == q->size && running) pthread_cond_wait(&full, &Qmutex);
    if (!running) return false;
    Node node = newNode(elem);
    if (q->length == 0) {
        q->front = q->back = node;
    } else {
        Node old_back = q->back;
        old_back->next = node;
        node->prev = old_back;
        q->back = node;
    }
    q->length++;
    // Unlock thread
    pthread_mutex_unlock(&Qmutex);
	pthread_cond_broadcast(&empty);
    return true;
}
// Pop from the end of the queue
bool queue_pop(queue_t *q, void **elem) {
    // Lock thread
    pthread_mutex_lock(&Qmutex);
    // Wait if there is nothing in the queue
	while (q->length == 0 && running) pthread_cond_wait(&empty, &Qmutex);
    if (!running) return false;
    // Get the client and save it to elem
    Node return_node;
    return_node = q->front;
    q->front = return_node->next;
    q->length--;
    if (q->length == 0) q->front = q->back = NULL;
    void *request;
    request = return_node->data;
    *elem = request;
    freeNode(&return_node);
    // Unlock thread
	pthread_mutex_unlock(&Qmutex);
	pthread_cond_broadcast(&full);
    return true;
}
// Clears the queue
void clear(queue_t *q) {
    void *elem;
    for (int i=0; i < q->length; i++) {
        queue_pop(q, &elem);
    }
    q->length = 0;
    q->front = q->back = NULL;
}

pthread_mutex_t *lock_mutex() {
    return &Qmutex;
}
pthread_cond_t *lock_full() {
    return &full;
}
pthread_cond_t *lock_empty () {
    return &empty;
}

bool queue_full(queue_t *q) {
    return q->length == q->size;
}
bool queue_empty (queue_t *q) {
    return q->length == 0;
}
