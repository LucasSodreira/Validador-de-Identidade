/**
 * @file queue.h
 * @brief Implementação de fila (FIFO) para IDs únicos
 * 
 * Esta fila implementa a política First In, First Out (FIFO) usando
 * uma lista encadeada simples com ponteiros para início e fim.
 */

#ifndef QUEUE_H
#define QUEUE_H

#include "utils.h"
#include <stdlib.h>

/**
 * @struct QueueNode
 * @brief Nó individual da fila
 */
typedef struct QueueNode {
    id_t data;                  /**< Dados armazenados no nó */
    struct QueueNode* next;     /**< Ponteiro para o próximo nó */
} QueueNode;

/**
 * @struct Queue
 * @brief Estrutura principal da fila
 */
typedef struct {
    QueueNode *front, *rear;    /**< Ponteiros para início e fim da fila */
    size_t size;                /**< Número atual de elementos na fila */
} Queue;

/**
 * @brief Cria uma nova fila vazia
 * @return Ponteiro para a fila criada, ou NULL em caso de erro
 */
Queue* queue_create();

/**
 * @brief Destrói uma fila e libera toda a memória
 * @param queue Ponteiro para a fila a ser destruída
 */
void queue_destroy(Queue* queue);

/**
 * @brief Adiciona um elemento no final da fila
 * @param queue Ponteiro para a fila
 * @param data Dados a serem adicionados
 */
void queue_enqueue(Queue* queue, id_t data);

/**
 * @brief Remove e retorna o elemento do início da fila
 * @param queue Ponteiro para a fila
 * @param data Ponteiro onde o dados removido será armazenado
 * @return 1 se bem-sucedido, 0 se a fila estiver vazia
 */
int queue_dequeue(Queue* queue, id_t* data);

/**
 * @brief Verifica se a fila está vazia
 * @param queue Ponteiro para a fila
 * @return 1 se vazia, 0 caso contrário
 */
int queue_is_empty(Queue* queue);

#endif // QUEUE_H
