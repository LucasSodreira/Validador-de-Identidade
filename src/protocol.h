#ifndef PROTOCOL_H
#define PROTOCOL_H

// Protocol commands and responses
#define CMD_GET "GET"
#define CMD_GETB "GETB"
#define RESP_EMPTY "EMPTY\n"

// Maximum number of IDs per batch response
#define MAX_BATCH_SIZE 1024

#endif // PROTOCOL_H