#include "parse.h"
#include "cmd.h"
#include "ptr_array.h"
#include "token.h"
#include "xmalloc.h"

#include <wordexp.h>

static struct {
    const PtrArray *tokens;
    size_t current;
    PtrArray *cmds;
} parser;

static void init(const PtrArray *tokens) {
    parser.tokens = tokens;
    parser.current = 0;
    parser.cmds = ptr_array_create();
}

static const Token *peek(void) {
    return ptr_array_get_const(parser.tokens, parser.current);
}

static bool is_at_end(void) {
    return peek()->type == TOKEN_EOF;
}

static const Token *previous(void) {
    return ptr_array_get_const(parser.tokens, parser.current - 1);
}

static const Token *advance(void) {
    if (!is_at_end()) {
        parser.current++;
    }
    return previous();
}

static bool match(TokenType type) {
    if (peek()->type != type) {
        return false;
    }
    advance();
    return true;
}

static void expand_word(PtrArray *arguments, const char *lexeme) {
    static wordexp_t we;
    wordexp(lexeme, &we, WRDE_REUSE);
    for (size_t i = 0; i < we.we_wordc; i++) {
        ptr_array_append(arguments, xstrdup(we.we_wordv[i]));
    }
}

static Cmd *command(void) {
    PtrArray *arguments = ptr_array_create();
    while (match(TOKEN_WORD)) {
        expand_word(arguments, previous()->lexeme);
    }
    return cmd_create(arguments);
}

PtrArray *parse(const PtrArray *tokens) {
    init(tokens);
    if (!is_at_end()) {
        do {
            ptr_array_append(parser.cmds, command());
        } while (match(TOKEN_OR));
    }
    return parser.cmds;
}
