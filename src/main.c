#include <err.h>
#include <readline/readline.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static int argc;
static char *argv[4096];

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

static char *find_executable(const char *name) {
    char *dirs = strdup(getenv("PATH"));
    char *dir = strtok(dirs, ":");
    while (dir != NULL) {
        size_t pathsz = strlen(dir) + strlen(name) + 2;
        char *path = malloc(pathsz);
        snprintf(path, pathsz, "%s/%s", dir, name);

        if (access(path, X_OK) == 0) {
            free(dirs);
            return path;
        }
        free(path);

        dir = strtok(NULL, ":");
    }
    free(dirs);
    return NULL;
}

static void cmd_type(void) {
    for (int i = 1; i < argc; i++) {
        const char *name = argv[i];
        if (is_builtin(name)) {
            printf("%s is a shell builtin\n", name);
            continue;
        }

        char *path = find_executable(name);
        if (path != NULL) {
            printf("%s is %s\n", name, path);
            free(path);
            continue;
        }

        printf("%s: not found\n", name);
    }
}

int main(void) {
    char *line;
    while ( (line = readline("$ ")) != NULL) {
        parse_line(line);

        const char *cmd = argv[0];
        if (strcmp(cmd, "echo") == 0) {
            cmd_echo();
        } else if (strcmp(cmd, "exit") == 0) {
            cmd_exit();
        } else if (strcmp(cmd, "type") == 0) {
            cmd_type();
        } else {
            char *path = find_executable(cmd);
            if (path == NULL) {
                fprintf(stderr, "%s: command not found\n", cmd);
                continue;
            }

            pid_t pid = fork();
            if (pid < 0) {
                err(EXIT_FAILURE, "cannot fork");
            } else if (pid == 0) {
                execvp(path, argv);
                err(EXIT_FAILURE, "execvp");
            }
            wait(NULL);
            free(path);
        }

        free(line);
    }

    exit(EXIT_SUCCESS);
}
