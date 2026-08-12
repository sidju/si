#ifndef SI_LEXER_H
#define SI_LEXER_H

#include <stddef.h>

typedef enum {
    SI_TOKEN_EOF = 0,
    SI_TOKEN_NEWLINE,
    SI_TOKEN_NUMBER,
    SI_TOKEN_IDENT,
    SI_TOKEN_INVALID
} SiTokenType;

typedef struct {
    SiTokenType type;
    const char *start;
    size_t length;
    double number_value;
} SiToken;

typedef struct {
    const char *input;
    size_t length;
    size_t offset;
} SiLexer;

void si_lexer_init(SiLexer *lexer, const char *input);
SiToken si_lexer_next(SiLexer *lexer);

#endif
