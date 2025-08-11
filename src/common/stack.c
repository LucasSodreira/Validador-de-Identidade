#include "stack.h"
#include <stdlib.h>

Stack* stack_create() {
    Stack* stack = (Stack*)malloc(sizeof(Stack));
    if (!stack) return NULL;
    stack->top = NULL;
    stack->size = 0;
    return stack;
}

void stack_destroy(Stack* stack) {
    if (!stack) return;
    StackNode* current = stack->top;
    while (current != NULL) {
        StackNode* temp = current;
        current = current->next;
        free(temp);
    }
    free(stack);
}

void stack_push(Stack* stack, id_t data) {
    StackNode* newNode = (StackNode*)malloc(sizeof(StackNode));
    if (!newNode) return;
    newNode->data = data;
    newNode->next = stack->top;
    stack->top = newNode;
    stack->size++;
}

int stack_pop(Stack* stack, id_t* data) {
    if (stack_is_empty(stack)) {
        return 0; // Failure
    }
    StackNode* temp = stack->top;
    *data = temp->data;
    stack->top = stack->top->next;
    free(temp);
    stack->size--;
    return 1; // Success
}

int stack_is_empty(Stack* stack) {
    return stack->top == NULL;
}
