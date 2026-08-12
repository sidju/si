#ifndef SI_PARSER_H
#define SI_PARSER_H

#include <stddef.h>

#include "si/lexer.h"

typedef enum {
    SI_OP_PUSH = 0,
    SI_OP_ADD,
    SI_OP_SUB,
    SI_OP_MUL,
    SI_OP_DIV,
    SI_OP_DUP,
    SI_OP_DROP,
    SI_OP_SWAP,
    SI_OP_PRINT
} SiOpCode;

typedef struct {
    SiOpCode op;
    double operand;
} SiInstruction;

typedef struct {
    SiInstruction *items;
    size_t count;
    size_t capacity;
} SiProgram;

typedef struct {
    int ok;
    char message[256];
} SiParseResult;

void si_program_init(SiProgram *program);
void si_program_free(SiProgram *program);
void si_parse_script(const char *script, SiProgram *program, SiParseResult *result);

#endif
