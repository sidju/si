#include "si/lexer.h"

#include <stdlib.h>
#include <string.h>

static int si_is_combining_mark(unsigned int cp) {
    return (cp >= 0x0300u && cp <= 0x036Fu) ||
           (cp >= 0x1AB0u && cp <= 0x1AFFu) ||
           (cp >= 0x1DC0u && cp <= 0x1DFFu) ||
           (cp >= 0x20D0u && cp <= 0x20FFu) ||
           (cp >= 0xFE20u && cp <= 0xFE2Fu);
}

static int si_is_variation_selector(unsigned int cp) {
    return (cp >= 0xFE00u && cp <= 0xFE0Fu) || (cp >= 0xE0100u && cp <= 0xE01EFu);
}

static size_t si_utf8_decode(const char *s, size_t remaining, unsigned int *cp) {
    unsigned char b0;
    if (remaining == 0u) {
        return 0u;
    }
    b0 = (unsigned char)s[0];
    if (b0 < 0x80u) {
        *cp = b0;
        return 1u;
    }
    if ((b0 & 0xE0u) == 0xC0u && remaining >= 2u) {
        *cp = ((unsigned int)(b0 & 0x1Fu) << 6) | (unsigned int)(s[1] & 0x3Fu);
        return 2u;
    }
    if ((b0 & 0xF0u) == 0xE0u && remaining >= 3u) {
        *cp = ((unsigned int)(b0 & 0x0Fu) << 12) | ((unsigned int)(s[1] & 0x3Fu) << 6) |
              (unsigned int)(s[2] & 0x3Fu);
        return 3u;
    }
    if ((b0 & 0xF8u) == 0xF0u && remaining >= 4u) {
        *cp = ((unsigned int)(b0 & 0x07u) << 18) | ((unsigned int)(s[1] & 0x3Fu) << 12) |
              ((unsigned int)(s[2] & 0x3Fu) << 6) | (unsigned int)(s[3] & 0x3Fu);
        return 4u;
    }
    *cp = 0xFFFDu;
    return 1u;
}

static size_t si_next_grapheme_length(const char *s, size_t remaining) {
    size_t consumed;
    unsigned int cp;
    unsigned int prev;
    size_t n;
    if (remaining == 0u) {
        return 0u;
    }
    n = si_utf8_decode(s, remaining, &cp);
    if (n == 0u) {
        return 0u;
    }
    consumed = n;
    prev = cp;
    while (consumed < remaining) {
        n = si_utf8_decode(s + consumed, remaining - consumed, &cp);
        if (n == 0u) {
            break;
        }
        if (si_is_combining_mark(cp) || si_is_variation_selector(cp) || cp == 0x200Du ||
            prev == 0x200Du) {
            consumed += n;
            prev = cp;
            continue;
        }
        break;
    }
    return consumed;
}

void si_lexer_init(SiLexer *lexer, const char *input) {
    lexer->input = input;
    lexer->length = strlen(input);
    lexer->offset = 0u;
}

static int si_grapheme_is_space(const char *start, size_t length) {
    return length == 1u && (start[0] == ' ' || start[0] == '\t' || start[0] == '\r');
}

static int si_grapheme_is_newline(const char *start, size_t length) {
    return length == 1u && start[0] == '\n';
}

SiToken si_lexer_next(SiLexer *lexer) {
    SiToken token;
    size_t g_len;
    const char *begin;
    char *endptr;
    token.type = SI_TOKEN_EOF;
    token.start = lexer->input + lexer->offset;
    token.length = 0u;
    token.number_value = 0.0;

    while (lexer->offset < lexer->length) {
        g_len = si_next_grapheme_length(lexer->input + lexer->offset, lexer->length - lexer->offset);
        if (g_len == 0u) {
            lexer->offset++;
            continue;
        }
        begin = lexer->input + lexer->offset;
        if (si_grapheme_is_space(begin, g_len)) {
            lexer->offset += g_len;
            continue;
        }
        if (si_grapheme_is_newline(begin, g_len)) {
            token.type = SI_TOKEN_NEWLINE;
            token.start = begin;
            token.length = g_len;
            lexer->offset += g_len;
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
        g_len = si_next_grapheme_length(lexer->input + lexer->offset, lexer->length - lexer->offset);
        if (g_len == 0u) {
            break;
        }
        if (si_grapheme_is_space(lexer->input + lexer->offset, g_len) ||
            si_grapheme_is_newline(lexer->input + lexer->offset, g_len)) {
            break;
        }
        lexer->offset += g_len;
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
