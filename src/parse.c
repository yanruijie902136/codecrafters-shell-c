#include "parse.h"
#include "ptr_array.h"
#include "token.h"
#include "xmalloc.h"

#include <wordexp.h>

static void expand_word(PtrArray *arguments, const char *lexeme) {
    static wordexp_t we;
    wordexp(lexeme, &we, WRDE_REUSE);
    for (size_t i = 0; i < we.we_wordc; i++) {
        ptr_array_append(arguments, xstrdup(we.we_wordv[i]));
    }
}

PtrArray *parse(const PtrArray *tokens) {
    PtrArray *arguments = ptr_array_create();
    size_t num_tokens = ptr_array_get_size(tokens);
    for (size_t i = 0; i < num_tokens; i++) {
        const Token *token = ptr_array_get(tokens, i);
        if (token->type == TOKEN_WORD) {
            expand_word(arguments, token->lexeme);
        }
    }
    return arguments;
}
