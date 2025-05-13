#include <err.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "autocmp.h"
#include "cmd.h"
#include "parse.h"
#include "ptr_array.h"
#include "scan.h"
#include "token.h"

static PtrArray *parse_line_to_args(const char *line) {
    PtrArray *tokens = scan(line);
    PtrArray *arguments = parse(tokens);
    ptr_array_destroy(tokens, token_destroy);
    return arguments;
}

int main(void) {
    rl_attempted_completion_function = shell_completion;

    char *line;
    while ( (line = readline("$ ")) != NULL) {
        PtrArray *arguments = parse_line_to_args(line);
        free(line);
        execute(arguments);
        ptr_array_destroy(arguments, free);
    }

    exit(EXIT_SUCCESS);
}
