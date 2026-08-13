#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "si/stack.h"

/* Simple payload used for testing */
typedef struct {
    int id;
    char message[32];
} test_payload_t;

/* Helper: compare two payloads */
static int payload_eq(const test_payload_t *a, const test_payload_t *b)
{
    return a->id == b->id && strcmp(a->message, b->message) == 0;
}

/* Helper: compare two metadata structures */
static int meta_eq(const stack_meta_t *a, const stack_meta_t *b)
{
    return strcmp(a->type, b->type) == 0 && a->data_size == b->data_size;
}

int main(void)
{
    /* -------------------------------------------------------------
     * 1. Basic initialization – allocate on the stack, no malloc
     * ------------------------------------------------------------- */
    stack_t s;
    memset(&s, 0, sizeof s);               /* zero‑initialize */
    assert(stack_init(&s, 256) == 0);       /* allocate internal buffer */

    /* -------------------------------------------------------------
     * 2. Push a payload and retrieve it
     * ------------------------------------------------------------- */
    test_payload_t payload1 = { .id = 1, .message = "first" };
    stack_meta_t meta1 = { .data_size = sizeof(payload1), .type = "test_payload" };
    assert(stack_push(&s, &meta1, &payload1) == 0);

    /* Retrieve the element at step 0 (the top) */
    stack_meta_t retrieved_meta;
    void *data_ptr;
    assert(stack_get(&s, 0, &retrieved_meta, &data_ptr) == 0);
    assert(meta_eq(&retrieved_meta, &meta1));
    assert(data_ptr != NULL);
    assert(payload_eq((test_payload_t *)data_ptr, &payload1));

    /* -------------------------------------------------------------
     * 3. Push another element and step down to the first one
     * ------------------------------------------------------------- */
    test_payload_t payload2 = { .id = 2, .message = "second" };
    stack_meta_t meta2 = { .data_size = sizeof(payload2), .type = "test_payload" };
    assert(stack_push(&s, &meta2, &payload2) == 0);

    /* The first element is now one step down */
    stack_meta_t retrieved_meta2;
    void *data_ptr2;
    assert(stack_get(&s, 1, &retrieved_meta2, &data_ptr2) == 0);
    assert(meta_eq(&retrieved_meta2, &meta1));
    assert(data_ptr2 != NULL);
    assert(payload_eq((test_payload_t *)data_ptr2, &payload1));

    /* -------------------------------------------------------------
     * 4. Drop the top element
     * ------------------------------------------------------------- */
    /* Drop the element we just pushed (payload2) */
    assert(stack_drop(&s, 1) == 0);         /* drop exactly one entry */
    /* After dropping, the remaining element should be at the top */
    stack_meta_t retrieved_meta3;
    void *data_ptr3;
    assert(stack_get(&s, 0, &retrieved_meta3, &data_ptr3) == 0);
    assert(meta_eq(&retrieved_meta3, &meta1));
    assert(data_ptr3 != NULL);
    assert(payload_eq((test_payload_t *)data_ptr3, &payload1));

    /* -------------------------------------------------------------
     * 5. Overflow detection – use a separate stack with a tiny buffer
     * ------------------------------------------------------------- */
    stack_t small_s;
    memset(&small_s, 0, sizeof small_s);
    assert(stack_init(&small_s, 64) == 0);   /* tiny buffer */

    /* Push several integers until the next push must fail */
    int i;
    for (i = 0; i < 5; ++i) {
        stack_meta_t m = { .data_size = sizeof(int), .type = "int" };
        int v = i;
        if (stack_push(&small_s, &m, &v) != 0) {
            /* The first failure should happen after some pushes succeeded */
            assert(i > 0);
            break;
        }
    }
    /* One more push must definitely fail (overflow) */
    stack_meta_t m_fail = { .data_size = sizeof(int), .type = "int" };
    int v_fail = 99;
    assert(stack_push(&small_s, &m_fail, &v_fail) != 0);   /* overflow -> fail */

    /* -------------------------------------------------------------
     * 6. Out‑of‑range get
     * ------------------------------------------------------------- */
    stack_meta_t dummy_meta;
    void *dummy_ptr;
    /* Negative step */
    assert(stack_get(&s, -1, &dummy_meta, &dummy_ptr) != 0);
    /* Step larger than the element count */
    assert(stack_get(&s, 10, &dummy_meta, &dummy_ptr) != 0);

    /* -------------------------------------------------------------
     * 7. Out‑of‑range drop
     * ------------------------------------------------------------- */
    /* Push a single element onto a fresh stack */
    stack_t single_s;
    memset(&single_s, 0, sizeof single_s);
    assert(stack_init(&single_s, 256) == 0);
    stack_meta_t single_meta = { .data_size = sizeof(int), .type = "single" };
    int single_val = 42;
    assert(stack_push(&single_s, &single_meta, &single_val) == 0);
    /* Try to drop more elements than are present */
    assert(stack_drop(&single_s, 2) != 0);        /* drop too many */
    /* Negative drop count should also fail */
    assert(stack_drop(&single_s, -1) != 0);

    /* -------------------------------------------------------------
     * Cleanup
     * ------------------------------------------------------------- */
    stack_destroy(&s);
    stack_destroy(&small_s);
    stack_destroy(&single_s);

    printf("All stack unit tests passed.\n");
    return 0;
}