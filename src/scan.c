#include "scan.h"
#include "ptr_array.h"
#include "token.h"
#include "xmalloc.h"

#include <ctype.h>
#include <err.h>
#include <stdbool.h>
#include <stdlib.h>

static struct {
    const char *start;
    const char *current;
    PtrArray *tokens;
} scanner;

static void init(const char *line) {
    scanner.start = line;
    scanner.current = line;
    scanner.tokens = ptr_array_create();
}

static char peek(void) {
    return *scanner.current;
}

static bool is_at_end(void) {
    return peek() == '\0';
}

static char advance(void) {
    return *scanner.current++;
}

static char *get_lexeme(void) {
    size_t lexeme_length = scanner.current - scanner.start;
    return xstrndup(scanner.start, lexeme_length);
}

static void add_token(TokenType type) {
    Token *token = token_create(type, get_lexeme());
    ptr_array_append(scanner.tokens, token);
}

static void single_quote(void) {
    while (!is_at_end() && peek() != '\'') {
        advance();
    }
    if (is_at_end()) {
        errx(EXIT_FAILURE, "missing single quote");
    }
    advance();
}

static void scan_token(void) {
    if (isspace(peek())) {
        advance();
        return;
    }

    while (!is_at_end() && !isspace(peek())) {
        switch (advance()) {
            case '\'':
                single_quote();
                break;
            default:
                break;
        }
    }
    add_token(TOKEN_WORD);
}

PtrArray *scan(const char *line) {
    init(line);
    while (!is_at_end()) {
        scan_token();
        scanner.start = scanner.current;
    }
    add_token(TOKEN_EOF);
    return scanner.tokens;
}
