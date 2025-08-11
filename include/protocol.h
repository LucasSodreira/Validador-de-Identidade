/**
 * @file protocol.h
 * @brief Definições do protocolo de comunicação cliente-servidor
 * 
 * Define os comandos e respostas do protocolo de texto usado para
 * comunicação entre cliente e servidor no sistema de IDs únicos.
 */

#ifndef PROTOCOL_H
#define PROTOCOL_H

/**
 * @brief Comando para solicitar um único ID
 * 
 * O servidor responde com um ID ou "EMPTY" se não houver mais IDs.
 */
#define CMD_GET   "GET"

/**
 * @brief Comando para solicitar múltiplos IDs em lote
 * 
 * Formato: "GETB <n>" onde n é o número máximo de IDs desejados.
 * O servidor responde com "<count> <id1> <id2> ... <idN>" ou "EMPTY".
 */
#define CMD_GETB  "GETB"

/**
 * @brief Resposta quando não há mais IDs disponíveis
 * 
 * Enviada pelo servidor quando a estrutura de dados está vazia.
 */
#define RESP_EMPTY "EMPTY"

#endif // PROTOCOL_H
