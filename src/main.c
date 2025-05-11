#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    for ( ; ; ) {
        char *line = readline("$ ");
        if (line == NULL) {
            exit(EXIT_SUCCESS);
        }
        printf("%s: command not found\n", line);
        free(line);
    }
}
