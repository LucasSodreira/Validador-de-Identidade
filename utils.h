/**
 * @file utils.h
 * @brief Funções utilitárias para o projeto Validador de Identidades Únicas
 * 
 * Este arquivo contém definições de tipos e funções auxiliares usadas
 * por todo o projeto, incluindo medição de tempo e embaralhamento de dados.
 */

#ifndef UTILS_H
#define UTILS_H

#include <time.h>
#include <stdlib.h>

/**
 * @typedef id_t
 * @brief Tipo usado para representar IDs únicos
 * 
 * Usa long long para suportar grandes quantidades de IDs (até 2^63-1)
 */
typedef long long id_t;

/**
 * @brief Obtém o tempo atual para benchmarking
 * 
 * Usa QueryPerformanceCounter no Windows e clock_gettime no Linux/Unix
 * para obter medições de tempo de alta precisão.
 * 
 * @return Tempo atual em segundos (com precisão de nanossegundos)
 */
double get_time();

/**
 * @brief Embaralha um array de IDs usando o algoritmo Fisher-Yates
 * 
 * Realiza um embaralhamento uniforme e imparcial do array fornecido.
 * A semente do gerador de números aleatórios deve ser definida antes
 * de chamar esta função.
 * 
 * @param array Ponteiro para o array de IDs a ser embaralhado
 * @param n Número de elementos no array
 * 
 * @note Complexidade: O(n)
 * @note Não chama srand() internamente - a semente deve ser definida externamente
 */
void shuffle(id_t *array, size_t n);

#endif // UTILS_H
