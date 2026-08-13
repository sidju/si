# Simple Makefile for the stack tests

CC      = gcc
CFLAGS  = -Wall -Wextra -std=c11
TARGET  = test_stack
SRCS    = src/stack.c test/test_stack.c
INCLUDES  = -I include

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^

clean:
	rm -f $(TARGET)