#include "queue.h"
#include <stdlib.h>

Queue* queue_create() {
    Queue* q = (Queue*)malloc(sizeof(Queue));
    if (!q) return NULL;
    q->front = q->rear = NULL;
    q->size = 0;
    return q;
}

void queue_destroy(Queue* queue) {
    if (!queue) return;
    QueueNode* current = queue->front;
    while (current != NULL) {
        QueueNode* temp = current;
        current = current->next;
        free(temp);
    }
    free(queue);
}

void queue_enqueue(Queue* queue, id_t data) {
    QueueNode* temp = (QueueNode*)malloc(sizeof(QueueNode));
    if (!temp) return;
    temp->data = data;
    temp->next = NULL;
    if (queue->rear == NULL) {
        queue->front = queue->rear = temp;
        queue->size++;
        return;
    }
    queue->rear->next = temp;
    queue->rear = temp;
    queue->size++;
}

int queue_dequeue(Queue* queue, id_t* data) {
    if (queue_is_empty(queue)) {
        return 0; // Failure
    }
    QueueNode* temp = queue->front;
    *data = temp->data;
    queue->front = queue->front->next;
    if (queue->front == NULL) {
        queue->rear = NULL;
    }
    free(temp);
    queue->size--;
    return 1; // Success
}

int queue_is_empty(Queue* queue) {
    return queue->front == NULL;
}
