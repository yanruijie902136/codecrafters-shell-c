#include <err.h>
#include <errno.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#include "parse.h"
#include "ptr_array.h"
#include "scan.h"
#include "token.h"
#include "xmalloc.h"

static PtrArray *parse_line_to_args(const char *line) {
    PtrArray *tokens = scan(line);
    PtrArray *arguments = parse(tokens);
    ptr_array_destroy(tokens, token_destroy);
    return arguments;
}

static void cmd_cd(const PtrArray *arguments) {
    const char *dst = ptr_array_get(arguments, 1);
    if (chdir(dst) < 0) {
        fprintf(stderr, "cd: %s: %s\n", dst, strerror(errno));
    }
}

static void cmd_echo(const PtrArray *arguments) {
    size_t num_args = ptr_array_get_size(arguments);
    for (size_t i = 1; i < num_args; i++) {
        if (i > 1) {
            printf(" ");
        }
        printf("%s", (const char *)ptr_array_get(arguments, i));
    }
    printf("\n");
}

static void cmd_exit(const PtrArray *arguments) {
    int status = atoi((const char *)ptr_array_get(arguments, 1));
    exit(status);
}

static void cmd_pwd(const PtrArray *arguments) {
    char *cwd = getcwd(NULL, 0);
    printf("%s\n", cwd);
    free(cwd);
}

static bool is_builtin(const char *name) {
    static const char *builtins[] = {"cd", "echo", "exit", "pwd", "type"};
    static const size_t num_builtins = sizeof(builtins) / sizeof(const char *);

    for (size_t i = 0; i < num_builtins; i++) {
        if (strcmp(name, builtins[i]) == 0) {
            return true;
        }
    }
    return false;
}

static char *path_join(const char *dir, const char *name) {
    size_t path_size = strlen(dir) + strlen(name) + 2;
    char *path = xmalloc(path_size);
    snprintf(path, path_size, "%s/%s", dir, name);
    return path;
}

static char *find_executable(const char *name) {
    char *dirs = xstrdup(getenv("PATH"));
    char *dir = strtok(dirs, ":");
    while (dir != NULL) {
        char *path = path_join(dir, name);
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

static void cmd_type(const PtrArray *arguments) {
    size_t num_args = ptr_array_get_size(arguments);
    for (size_t i = 1; i < num_args; i++) {
        const char *name = ptr_array_get(arguments, i);

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

static void execute(PtrArray *arguments) {
    if (ptr_array_is_empty(arguments)) {
        return;
    }

    const char *cmd_name = ptr_array_get(arguments, 0);
    if (strcmp(cmd_name, "cd") == 0) {
        cmd_cd(arguments);
    } else if (strcmp(cmd_name, "echo") == 0) {
        cmd_echo(arguments);
    } else if (strcmp(cmd_name, "exit") == 0) {
        cmd_exit(arguments);
    } else if (strcmp(cmd_name, "pwd") == 0) {
        cmd_pwd(arguments);
    } else if (strcmp(cmd_name, "type") == 0) {
        cmd_type(arguments);
    } else {
        char *path = find_executable(cmd_name);
        if (path == NULL) {
            fprintf(stderr, "%s: command not found\n", cmd_name);
            return;
        }

        ptr_array_append(arguments, NULL);
        if (fork() == 0) {
            execvp(path, (char **)ptr_array_to_c_array(arguments));
            err(EXIT_FAILURE, "execvp");
        }
        wait(NULL);
        free(path);
    }
}

int main(void) {
    char *line;
    while ( (line = readline("$ ")) != NULL) {
        PtrArray *arguments = parse_line_to_args(line);
        free(line);
        execute(arguments);
        ptr_array_destroy(arguments, free);
    }

    exit(EXIT_SUCCESS);
}
