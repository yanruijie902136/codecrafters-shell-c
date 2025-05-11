#include <limits.h>
#include <readline/readline.h>
#include <stdbool.h>
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

static void cmd_echo(void) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) {
            printf(" ");
        }
        printf("%s", argv[i]);
    }
    printf("\n");
}

static void cmd_exit(void) {
    int status = atoi(argv[1]);
    exit(status);
}

static bool is_builtin(const char *name) {
    static const char *builtins[] = {"echo", "exit", "type"};
    static const int num_builtins = sizeof(builtins) / sizeof(const char *);

    for (int i = 0; i < num_builtins; i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return true;
        }
    }
    return false;
}

static void cmd_type(void) {
    for (int i = 1; i < argc; i++) {
        const char *name = argv[i];
        if (is_builtin(name)) {
            printf("%s is a shell builtin\n", name);
        } else {
            printf("%s: not found\n", name);
        }
    }
}

int main(void) {
    for ( ; ; ) {
        char *line = readline("$ ");
        if (line == NULL) {     // End of input.
            exit(EXIT_SUCCESS);
        }

        parse_line(line);

        const char *cmd = argv[0];
        if (strcmp(cmd, "echo") == 0) {
            cmd_echo();
        } else if (strcmp(cmd, "exit") == 0) {
            cmd_exit();
        } else if (strcmp(cmd, "type") == 0) {
            cmd_type();
        } else {
            fprintf(stderr, "%s: command not found\n", cmd);
        }

        free(line);
    }
}
