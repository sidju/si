#include <stdio.h>
#include <string.h>

#include "grapheme.h"
#include "si/lexer.h"
#include "si/parser.h"
#include "si/runtime.h"

static int assert_true(int condition, const char *message) {
    if (!condition) {
        fprintf(stderr, "FAIL: %s\n", message);
        return 0;
    }
    return 1;
}

static int test_grapheme_aware_lexing(void) {
    SiLexer lexer;
    SiToken token;
    const char *script = "1 a\xCC\x81""dd\n";
    si_lexer_init(&lexer, script);

    token = si_lexer_next(&lexer);
    if (!assert_true(token.type == SI_TOKEN_NUMBER, "expected number token")) {
        return 0;
    }
    token = si_lexer_next(&lexer);
    if (!assert_true(token.type == SI_TOKEN_IDENT, "expected identifier token")) {
        return 0;
    }
    if (!assert_true(token.length == strlen("a\xCC\x81""dd"), "expected grapheme-preserving token length")) {
        return 0;
    }
    token = si_lexer_next(&lexer);
    if (!assert_true(token.type == SI_TOKEN_NEWLINE, "expected newline token")) {
        return 0;
    }
    return 1;
}

static int test_grapheme_iteration_support(void) {
    const char *grapheme = "a\xCC\x81";

    return assert_true(grapheme_next_character_break_utf8(grapheme, strlen(grapheme)) == strlen(grapheme),
                       "expected vendored library to iterate a composed grapheme as one cluster");
}

static int test_recursive_descent_parse(void) {
    SiProgram program;
    SiParseResult result;
    si_parse_script("1 2 add print\n", &program, &result);
    if (!assert_true(result.ok, "expected parse success")) {
        return 0;
    }
    if (!assert_true(program.count == 4u, "expected four instructions")) {
        si_program_free(&program);
        return 0;
    }
    if (!assert_true(program.items[0].op == SI_OP_PUSH && program.items[1].op == SI_OP_PUSH &&
                         program.items[2].op == SI_OP_ADD && program.items[3].op == SI_OP_PRINT,
                     "expected push push add print")) {
        si_program_free(&program);
        return 0;
    }
    si_program_free(&program);
    return 1;
}

static int test_runtime_execute(void) {
    FILE *out = tmpfile();
    FILE *err = tmpfile();
    char buffer[32];
    int rc;
    size_t nread;

    if (out == NULL || err == NULL) {
        if (out != NULL) {
            fclose(out);
        }
        if (err != NULL) {
            fclose(err);
        }
        fprintf(stderr, "FAIL: tmpfile failed\n");
        return 0;
    }

    rc = si_execute_script("3 4 add print\n", out, err);
    if (!assert_true(rc == 0, "expected runtime success")) {
        fclose(out);
        fclose(err);
        return 0;
    }

    rewind(out);
    memset(buffer, 0, sizeof(buffer));
    nread = fread(buffer, 1, sizeof(buffer) - 1u, out);
    buffer[nread] = '\0';
    fclose(out);
    fclose(err);

    return assert_true(strcmp(buffer, "7\n") == 0, "expected print output 7");
}

int main(void) {
    int ok = 1;
    ok = test_grapheme_iteration_support() && ok;
    ok = test_grapheme_aware_lexing() && ok;
    ok = test_recursive_descent_parse() && ok;
    ok = test_runtime_execute() && ok;
    if (!ok) {
        return 1;
    }
    puts("all tests passed");
    return 0;
}
