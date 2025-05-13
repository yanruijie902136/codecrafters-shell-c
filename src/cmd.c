#include "cmd.h"
#include "misc.h"
#include "ptr_array.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

static void cmd_cd(const PtrArray *arguments) {
    const char *dir = ptr_array_get(arguments, 1);
    if (chdir(dir) < 0) {
        fprintf(stderr, "cd: %s: %s\n", dir, strerror(errno));
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

static void execute_builtin(const PtrArray *arguments) {
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
    }
}

static void execute_external(const char *path, PtrArray *arguments) {
    ptr_array_append(arguments, NULL);
    execvp(path, (char **)ptr_array_get_c_array(arguments));
    err(EXIT_FAILURE, "execvp");
}

void execute(PtrArray *arguments) {
    const char *cmd_name = ptr_array_get(arguments, 0);
    if (is_builtin(cmd_name)) {
        execute_builtin(arguments);
        return;
    }

    char *path = find_executable(cmd_name);
    if (path == NULL) {
        fprintf(stderr, "%s: command not found\n", cmd_name);
        return;
    }

    if (fork() == 0) {
        execute_external(path, arguments);
    }
    wait(NULL);
    free(path);
}
