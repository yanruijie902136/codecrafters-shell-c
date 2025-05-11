#include <limits.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef ARG_MAX
#define ARG_MAX 4096
#endif

static int argc;
static char *argv[ARG_MAX];

static void parse_line(char *line) {
    argc = 0;
    char *arg = strtok(line, " ");
    while (arg != NULL) {
        argv[argc++] = arg;
        arg = strtok(NULL, " ");
    }
    argv[argc] = NULL;
}

static void cmd_exit(void) {
    int status = atoi(argv[1]);
    exit(status);
}

int main(void) {
    for ( ; ; ) {
        char *line = readline("$ ");
        if (line == NULL) {     // End of input.
            exit(EXIT_SUCCESS);
        }

        parse_line(line);

        const char *cmd = argv[0];
        if (strcmp(cmd, "exit") == 0) {
            cmd_exit();
        } else {
            fprintf(stderr, "%s: command not found\n", cmd);
        }

        free(line);
    }
}
