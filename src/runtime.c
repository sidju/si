#include "si/runtime.h"

#include <stdio.h>

#include "si/parser.h"

typedef struct {
    double items[1024];
    size_t count;
} SiStack;

static int si_stack_push(SiStack *stack, double value) {
    if (stack->count >= (sizeof(stack->items) / sizeof(stack->items[0]))) {
        return 0;
    }
    stack->items[stack->count++] = value;
    return 1;
}

static int si_stack_pop(SiStack *stack, double *value) {
    if (stack->count == 0u) {
        return 0;
    }
    *value = stack->items[--stack->count];
    return 1;
}

int si_execute_script(const char *script, FILE *out, FILE *err) {
    SiProgram program;
    SiParseResult parse_result;
    SiStack stack;
    size_t i;
    double a;
    double b;
    stack.count = 0u;

    si_parse_script(script, &program, &parse_result);
    if (!parse_result.ok) {
        fprintf(err, "%s\n", parse_result.message);
        return 1;
    }

    for (i = 0u; i < program.count; ++i) {
        switch (program.items[i].op) {
            case SI_OP_PUSH:
                if (!si_stack_push(&stack, program.items[i].operand)) {
                    fprintf(err, "runtime error: stack overflow\n");
                    si_program_free(&program);
                    return 1;
                }
                break;
            case SI_OP_ADD:
                if (!si_stack_pop(&stack, &a) || !si_stack_pop(&stack, &b) || !si_stack_push(&stack, b + a)) {
                    fprintf(err, "runtime error: add requires two values\n");
                    si_program_free(&program);
                    return 1;
                }
                break;
            case SI_OP_SUB:
                if (!si_stack_pop(&stack, &a) || !si_stack_pop(&stack, &b) || !si_stack_push(&stack, b - a)) {
                    fprintf(err, "runtime error: sub requires two values\n");
                    si_program_free(&program);
                    return 1;
                }
                break;
            case SI_OP_MUL:
                if (!si_stack_pop(&stack, &a) || !si_stack_pop(&stack, &b) || !si_stack_push(&stack, b * a)) {
                    fprintf(err, "runtime error: mul requires two values\n");
                    si_program_free(&program);
                    return 1;
                }
                break;
            case SI_OP_DIV:
                if (!si_stack_pop(&stack, &a) || !si_stack_pop(&stack, &b) || a == 0.0 ||
                    !si_stack_push(&stack, b / a)) {
                    fprintf(err, "runtime error: div requires two values and non-zero divisor\n");
                    si_program_free(&program);
                    return 1;
                }
                break;
            case SI_OP_DUP:
                if (stack.count == 0u || !si_stack_push(&stack, stack.items[stack.count - 1u])) {
                    fprintf(err, "runtime error: dup requires one value\n");
                    si_program_free(&program);
                    return 1;
                }
                break;
            case SI_OP_DROP:
                if (!si_stack_pop(&stack, &a)) {
                    fprintf(err, "runtime error: drop requires one value\n");
                    si_program_free(&program);
                    return 1;
                }
                break;
            case SI_OP_SWAP:
                if (!si_stack_pop(&stack, &a) || !si_stack_pop(&stack, &b) || !si_stack_push(&stack, a) ||
                    !si_stack_push(&stack, b)) {
                    fprintf(err, "runtime error: swap requires two values\n");
                    si_program_free(&program);
                    return 1;
                }
                break;
            case SI_OP_PRINT:
                if (!si_stack_pop(&stack, &a)) {
                    fprintf(err, "runtime error: print requires one value\n");
                    si_program_free(&program);
                    return 1;
                }
                fprintf(out, "%g\n", a);
                break;
        }
    }

    si_program_free(&program);
    return 0;
}
