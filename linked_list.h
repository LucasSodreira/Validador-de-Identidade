/**
 * @file linked_list.h
 * @brief Implementação de lista encadeada simples para IDs únicos
 * 
 * Esta lista encadeada implementa inserção no início e remoção do início,
 * funcionando de forma similar a uma pilha (LIFO).
 */

#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "utils.h"
#include <stdlib.h>

/**
 * @struct ListNode
 * @brief Nó individual da lista encadeada
 */
typedef struct ListNode {
    id_t data;                  /**< Dados armazenados no nó */
    struct ListNode* next;      /**< Ponteiro para o próximo nó */
} ListNode;

/**
 * @struct LinkedList
 * @brief Estrutura principal da lista encadeada
 */
typedef struct {
    ListNode* head;             /**< Ponteiro para o primeiro nó da lista */
    size_t size;                /**< Número atual de elementos na lista */
} LinkedList;

/**
 * @brief Cria uma nova lista encadeada vazia
 * @return Ponteiro para a lista criada, ou NULL em caso de erro
 */
LinkedList* list_create();

/**
 * @brief Destrói uma lista encadeada e libera toda a memória
 * @param list Ponteiro para a lista a ser destruída
 */
void list_destroy(LinkedList* list);

/**
 * @brief Adiciona um elemento no início da lista
 * @param list Ponteiro para a lista
 * @param data Dados a serem adicionados
 */
void list_add(LinkedList* list, id_t data);

/**
 * @brief Remove e retorna o elemento do início da lista
 * @param list Ponteiro para a lista
 * @param data Ponteiro onde o dados removido será armazenado
 * @return 1 se bem-sucedido, 0 se a lista estiver vazia
 */
int list_remove_head(LinkedList* list, id_t* data);

/**
 * @brief Verifica se a lista está vazia
 * @param list Ponteiro para a lista
 * @return 1 se vazia, 0 caso contrário
 */
int list_is_empty(LinkedList* list);

#endif // LINKED_LIST_H
