#include "si/lexer.h"

#include <stdlib.h>
#include <string.h>

#include "grapheme.h"

static size_t si_next_grapheme_length(const char *start, size_t remaining) {
    size_t len = grapheme_next_character_break_utf8(start, remaining);
    return len == 0u ? 1u : len;
}

void si_lexer_init(SiLexer *lexer, const char *input) {
    lexer->input = input;
    lexer->length = strlen(input);
    lexer->offset = 0u;
}

static int si_is_space(uint_least32_t cp) {
    return cp == ' ' || cp == '\t' || cp == '\r';
}

static int si_is_newline(uint_least32_t cp) {
    return cp == '\n';
}

SiToken si_lexer_next(SiLexer *lexer) {
    SiToken token;
    const char *begin;
    char *endptr;
    uint_least32_t cp;
    size_t cp_len;
    size_t remaining;
    token.type = SI_TOKEN_EOF;
    token.start = lexer->input + lexer->offset;
    token.length = 0u;
    token.number_value = 0.0;

    while (lexer->offset < lexer->length) {
        remaining = lexer->length - lexer->offset;
        cp_len = si_next_grapheme_length(lexer->input + lexer->offset, remaining);
        (void)grapheme_decode_utf8(lexer->input + lexer->offset, remaining, &cp);
        if (si_is_space(cp)) {
            lexer->offset += cp_len;
            continue;
        }
        if (si_is_newline(cp)) {
            token.type = SI_TOKEN_NEWLINE;
            token.start = lexer->input + lexer->offset;
            token.length = cp_len;
            lexer->offset += cp_len;
            return token;
        }
        break;
    }

    if (lexer->offset >= lexer->length) {
        token.type = SI_TOKEN_EOF;
        return token;
    }

    begin = lexer->input + lexer->offset;
    while (lexer->offset < lexer->length) {
        remaining = lexer->length - lexer->offset;
        cp_len = si_next_grapheme_length(lexer->input + lexer->offset, remaining);
        (void)grapheme_decode_utf8(lexer->input + lexer->offset, remaining, &cp);
        if (si_is_space(cp) || si_is_newline(cp)) {
            break;
        }
        lexer->offset += cp_len;
    }

    token.start = begin;
    token.length = (size_t)((lexer->input + lexer->offset) - begin);

    token.number_value = strtod(begin, &endptr);
    if (endptr == begin + (ptrdiff_t)token.length) {
        token.type = SI_TOKEN_NUMBER;
        return token;
    }

    if (token.length > 0u) {
        token.type = SI_TOKEN_IDENT;
    } else {
        token.type = SI_TOKEN_INVALID;
    }
    return token;
}
