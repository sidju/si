#ifndef STACK_H
#define STACK_H

#include <stddef.h>

/* Stack metadata – data size and a short type identifier */
typedef struct {
    size_t data_size;      /* Size of the payload */
    char   type[32];       /* Type name (null‑terminated) */
} stack_meta_t;

/* Opaque stack type – now with a known definition */
typedef struct stack {
    char *base;            /* Start of the allocated buffer               */
    size_t capacity;       /* Total capacity of the buffer, in bytes      */
    char *top;             /* Points to the metadata of the current top   */
    size_t count;          /* Number of elements currently stored         */
} stack_t;

/* Stack operations */
int stack_init(stack_t *s, size_t initial_capacity);   /* create a stack */
int stack_push(stack_t *s, const stack_meta_t *meta, const void *data);
/*   Push an element; metadata describes the payload that follows */
int stack_get(stack_t *s, int steps, stack_meta_t *meta, void **data_out);
/*   Retrieve metadata and data N steps down (0 = top) */
int stack_drop(stack_t *s, int steps);
/*   Drop N elements from the top */
void stack_destroy(stack_t *s);                        /* release resources */

#endif /* STACK_H */