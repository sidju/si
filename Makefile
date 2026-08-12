CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Wpedantic -Iinclude -Ivendor/libgrapheme
LDFLAGS ?=

BUILD_DIR := build
TARGET := $(BUILD_DIR)/si
TEST_TARGET := $(BUILD_DIR)/test_runtime

VENDORED_SRC := vendor/libgrapheme/src/utf8.c vendor/libgrapheme/src/util.c vendor/libgrapheme/src/character.c vendor/libgrapheme/src/word.c
COMMON_SRC := src/lexer.c src/parser.c src/runtime.c $(VENDORED_SRC)
APP_SRC := src/main.c $(COMMON_SRC)
TEST_SRC := tests/test_runtime.c $(COMMON_SRC)

.PHONY: all clean test

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(APP_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(APP_SRC) -o $@ $(LDFLAGS)

$(TEST_TARGET): $(TEST_SRC) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(TEST_SRC) -o $@ $(LDFLAGS)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -rf $(BUILD_DIR)
