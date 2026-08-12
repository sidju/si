#include "si/parser.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void si_set_error(SiParseResult *result, const char *message) {
    result->ok = 0;
    snprintf(result->message, sizeof(result->message), "%s", message);
}

void si_program_init(SiProgram *program) {
    program->items = NULL;
    program->count = 0u;
    program->capacity = 0u;
}

void si_program_free(SiProgram *program) {
    free(program->items);
    program->items = NULL;
    program->count = 0u;
    program->capacity = 0u;
}

static int si_program_push(SiProgram *program, SiInstruction instruction) {
    SiInstruction *next;
    size_t next_capacity;
    if (program->count == program->capacity) {
        next_capacity = program->capacity == 0u ? 16u : program->capacity * 2u;
        next = (SiInstruction *)realloc(program->items, next_capacity * sizeof(SiInstruction));
        if (next == NULL) {
            return 0;
        }
        program->items = next;
        program->capacity = next_capacity;
    }
    program->items[program->count++] = instruction;
    return 1;
}

static int si_token_equals(const SiToken *token, const char *value) {
    size_t len;
    len = strlen(value);
    return token->length == len && strncmp(token->start, value, len) == 0;
}

static int si_parse_instruction(const SiToken *token, SiInstruction *instruction) {
    if (token->type == SI_TOKEN_NUMBER) {
        instruction->op = SI_OP_PUSH;
        instruction->operand = token->number_value;
        return 1;
    }
    if (token->type != SI_TOKEN_IDENT) {
        return 0;
    }
    if (si_token_equals(token, "add")) {
        instruction->op = SI_OP_ADD;
    } else if (si_token_equals(token, "sub")) {
        instruction->op = SI_OP_SUB;
    } else if (si_token_equals(token, "mul")) {
        instruction->op = SI_OP_MUL;
    } else if (si_token_equals(token, "div")) {
        instruction->op = SI_OP_DIV;
    } else if (si_token_equals(token, "dup")) {
        instruction->op = SI_OP_DUP;
    } else if (si_token_equals(token, "drop")) {
        instruction->op = SI_OP_DROP;
    } else if (si_token_equals(token, "swap")) {
        instruction->op = SI_OP_SWAP;
    } else if (si_token_equals(token, "print")) {
        instruction->op = SI_OP_PRINT;
    } else {
        return 0;
    }
    instruction->operand = 0.0;
    return 1;
}

static void si_parse_instruction_list_recursive(SiLexer *lexer, SiProgram *program, SiToken current,
                                                SiParseResult *result) {
    SiInstruction instruction;
    SiToken next;
    if (!result->ok) {
        return;
    }
    if (current.type == SI_TOKEN_EOF) {
        return;
    }
    if (current.type == SI_TOKEN_NEWLINE) {
        next = si_lexer_next(lexer);
        si_parse_instruction_list_recursive(lexer, program, next, result);
        return;
    }
    if (!si_parse_instruction(&current, &instruction)) {
        si_set_error(result, "parse error: unknown instruction");
        return;
    }
    if (!si_program_push(program, instruction)) {
        si_set_error(result, "parse error: out of memory");
        return;
    }
    next = si_lexer_next(lexer);
    si_parse_instruction_list_recursive(lexer, program, next, result);
}

void si_parse_script(const char *script, SiProgram *program, SiParseResult *result) {
    SiLexer lexer;
    SiToken first;
    si_program_init(program);
    result->ok = 1;
    result->message[0] = '\0';

    si_lexer_init(&lexer, script);
    first = si_lexer_next(&lexer);
    si_parse_instruction_list_recursive(&lexer, program, first, result);
    if (!result->ok) {
        si_program_free(program);
    }
}
