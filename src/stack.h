/**
 * @file stack.h
 * @brief Implementação de pilha (LIFO) para IDs únicos
 *
 * Esta pilha implementa a política Last In, First Out (LIFO) usando
 * uma lista encadeada simples.
 */

#ifndef STACK_H
#define STACK_H

#include "utils.h"
#include <stdlib.h>

/**
 * @struct StackNode
 * @brief Nó individual da pilha
 */
typedef struct StackNode {
    id_t data;                  /**< Dados armazenados no nó */
    struct StackNode* next;     /**< Ponteiro para o próximo nó */
} StackNode;

/**
 * @struct Stack
 * @brief Estrutura principal da pilha
 */
typedef struct {
    StackNode* top;             /**< Ponteiro para o topo da pilha */
    size_t size;                /**< Número atual de elementos na pilha */
} Stack;

/**
 * @brief Cria uma nova pilha vazia
 * @return Ponteiro para a pilha criada, ou NULL em caso de erro
 */
Stack* stack_create();

/**
 * @brief Destrói uma pilha e libera toda a memória
 * @param stack Ponteiro para a pilha a ser destruída
 */
void stack_destroy(Stack* stack);

/**
 * @brief Adiciona um elemento no topo da pilha
 * @param stack Ponteiro para a pilha
 * @param data Dados a serem adicionados
 */
void stack_push(Stack* stack, id_t data);

/**
 * @brief Remove e retorna o elemento do topo da pilha
 * @param stack Ponteiro para a pilha
 * @param data Ponteiro onde o dados removido será armazenado
 * @return 1 se bem-sucedido, 0 se a pilha estiver vazia
 */
int stack_pop(Stack* stack, id_t* data);

/**
 * @brief Verifica se a pilha está vazia
 * @param stack Ponteiro para a pilha
 * @return 1 se vazia, 0 caso contrário
 */
int stack_is_empty(Stack* stack);

#endif // STACK_H
