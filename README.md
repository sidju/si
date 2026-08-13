# SI Stack Implementation

This is a simple stack implementation with metadata support, written in C.

## Features

- LIFO (Last In, First Out) data structure
- Stores metadata along with each element (type and size)
- Single contiguous memory buffer for both metadata and payloads
- Overflow detection
- Bounds checking for get and drop operations
- Proper memory management with stack_init() and stack_destroy()

## Files

```
.
├── include/
│   └── si/
│       └── stack.h    # Header file
├── src/
│   └── stack.c        # Implementation
└── test_stack.c         # Unit tests
```

## API

### Types

```c
typedef struct {
    size_t data_size;      // Size of the payload
    char   type[32];       // Type name (null-terminated)
} stack_meta_t;

typedef struct stack {
    char *base;            // Start of allocated buffer
    size_t capacity;     // Total capacity of buffer, in bytes
    char *top;             // Points to metadata of current top
    size_t count;           // Number of elements stored
} stack_t;
```

### Functions

```c
int stack_init(stack_t *s, size_t initial_capacity);
int stack_push(stack_t *s, const stack_meta_t *meta, const void *data);
int stack_get(stack_t *s, int steps, stack_meta_t *meta, void **data_out);
int stack_drop(stack_t *s, int steps);
void stack_destroy(stack_t *s);
```

## Usage

Initialize stack:
```c
stack_t s;
stack_init(&s, 1024);  // 1KB buffer
```

Push data:
```c
stack_meta_t meta = { .data_size = sizeof(int), .type = "int" };
int value = 42;
stack_push(&s, &meta, &value);
```

Get data:
```c
stack_meta_t retrieved_meta;
void *data_ptr;
stack_get(&s, 0, &retrieved_meta, &data_ptr); // Get top element
```

Cleanup:
```c
stack_destroy(&s);
```

## Build and Test

Build with:
```
make
```

Run tests:
```
./test_stack
```