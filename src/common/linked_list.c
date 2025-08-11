#include "linked_list.h"
#include <stdlib.h>

LinkedList* list_create() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    if (!list) return NULL;
    list->head = NULL;
    list->size = 0;
    return list;
}

void list_destroy(LinkedList* list) {
    if (!list) return;
    ListNode* current = list->head;
    while (current != NULL) {
        ListNode* temp = current;
        current = current->next;
        free(temp);
    }
    free(list);
}

void list_add(LinkedList* list, id_t data) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (!newNode) return;
    newNode->data = data;
    newNode->next = list->head;
    list->head = newNode;
    list->size++;
}

int list_remove_head(LinkedList* list, id_t* data) {
    if (list_is_empty(list)) {
        return 0; // Failure
    }
    ListNode* temp = list->head;
    *data = temp->data;
    list->head = list->head->next;
    free(temp);
    list->size--;
    return 1; // Success
}

int list_is_empty(LinkedList* list) {
    return list->head == NULL;
}
