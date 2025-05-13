#ifndef CODECRAFTERS_SHELL_TOKEN_H_INCLUDED
#define CODECRAFTERS_SHELL_TOKEN_H_INCLUDED

typedef enum {
    TOKEN_WORD,
    TOKEN_OR,
    TOKEN_EOF,
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
} Token;

// Allocates memory for a token.
Token *token_create(TokenType type, char *lexeme);

// Deallocates memory for a token.
void token_destroy(void *token);

#endif
