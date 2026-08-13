#include "si/stack.h"
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

/* Compute the size (in bytes) of a single stack entry */
static size_t _entry_size(const stack_meta_t *meta)
{
    return sizeof(stack_meta_t) + meta->data_size;
}

/* Constructor – allocate a buffer of `initial_capacity` bytes */
int stack_init(stack_t *s, size_t initial_capacity)
{
    s->base = malloc(initial_capacity);
    if (!s->base)
        return -1;

    s->capacity = initial_capacity;
    s->top = s->base + initial_capacity;   /* Points just past the buffer */
    s->count = 0;
    return 0;
}

/* Destructor – release the internal buffer */
void stack_destroy(stack_t *s)
{
    free(s->base);
    s->base = NULL;
    s->top    = NULL;
    s->capacity = 0;
    s->count    = 0;
}

/* Push a new element.
 *
 *  meta – describes the payload (data size + type string)
 *  data – points to the actual payload
 *
 * Returns 0 on success, -1 on overflow or invalid arguments.
 */
int stack_push(stack_t *s, const stack_meta_t *meta, const void *data)
{
    if (!s || !meta || !data)
        return -1;

    size_t entry_sz = _entry_size(meta);

    /* Overflow check – the new top must stay inside the allocated buffer */
    if (s->top - entry_sz < s->base)
        return -1;   /* Not enough space */

    /* Make room for the entry by moving the top pointer downward */
    s->top -= entry_sz;

    /* Write metadata */
    stack_meta_t *dst_meta = (stack_meta_t *)s->top;
    strncpy(dst_meta->type, meta->type, sizeof(dst_meta->type) - 1);
    dst_meta->type[sizeof(dst_meta->type) - 1] = '\0';
    dst_meta->data_size = meta->data_size;

    /* Write the payload immediately after the metadata */
    void *dst_data = (char *)dst_meta + sizeof(stack_meta_t);
    memcpy(dst_data, data, meta->data_size);

    /* Update element count */
    s->count++;
    return 0;
}

/* Retrieve metadata and data N steps down from the top.
 *
 *  steps – number of entries to go down (0 = top)
 *  meta  – output pointer that receives the metadata
 *  data_out – output pointer that receives a pointer to the data
 *
 * Returns 0 on success, -1 if `steps` is out of range.
 */
int stack_get(stack_t *s, int steps, stack_meta_t *meta, void **data_out)
{
    if (!s || !meta || !data_out)
        return -1;
    if (steps < 0 || (size_t)steps >= s->count)
        return -1;   /* steps out of range */

    char *curr = s->top;
    for (int i = 0; i < steps; ++i) {
        stack_meta_t *curr_meta = (stack_meta_t *)curr;
        size_t sz = _entry_size(curr_meta);
        /* Move to the next (older) entry */
        char *next = curr + sz;
        if (next >= s->base + s->capacity)
            return -1;   /* Corrupted stack state */
        curr = next;
    }

    /* Now `curr` points at the desired metadata */
    stack_meta_t *target_meta = (stack_meta_t *)curr;
    *meta = *target_meta;   /* copy metadata to output struct */

    if (data_out) {
        *data_out = (char *)target_meta + sizeof(stack_meta_t);
    }
    return 0;
}

/* Drop N elements from the top of the stack.
 *
 *  steps – number of entries to drop
 *
 * Returns 0 on success, -1 if `steps` is invalid.
 */
int stack_drop(stack_t *s, int steps)
{
    if (!s || steps < 0 || (size_t)steps > s->count)
        return -1;

    for (int i = 0; i < steps; ++i) {
        stack_meta_t *meta = (stack_meta_t *)s->top;
        size_t sz = _entry_size(meta);
        s->top += sz;        /* move upward, discarding this entry */
        s->count--;
    }
    return 0;
}