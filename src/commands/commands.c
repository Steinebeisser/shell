#include "commands.h"
#include "builtins.h"
#include "externals.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "core/shell_print.h"

#include "config/config.h"
#include "config/alias.h"

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>


bool execute_command(int argc, char **argv) {
    if (argc == 0) return false;

    const char *alias_value = get__alias(argv[0]);

    int execute_argc = argc;
    char **execute_argv = argv;

    if (alias_value) {
        char **alias_args = NULL;
        int alias_argc = 0;
        if (!tokenize_alias_value(alias_value, &alias_args, &alias_argc)) return false;

        execute_argc = alias_argc + argc - 1;
        execute_argv = malloc((execute_argc + 1) * sizeof(char *));
        if (!execute_argv) return false;

        for (int i = 0; i < alias_argc; i++) {
            execute_argv[i] = alias_args[i];
        }
        for (int i = 1; i < argc; i++) {
            execute_argv[alias_argc + i - 1] = strdup(argv[i]);
        }
        execute_argv[execute_argc] = NULL;

        free(alias_args);

        char *full_alias_cmd = NULL;
        size_t total_length = 0;
        for (int i = 0; i < execute_argc; i++) {
            if (execute_argv[i]) total_length += strlen(execute_argv[i]) + 1;
        }
        full_alias_cmd = malloc(total_length + 1);
        if (full_alias_cmd) {
            full_alias_cmd[0] = '\0';
            for (int i = 0; i < execute_argc; i++) {
                if (execute_argv[i]) {
                    if (i > 0) strcat(full_alias_cmd, " ");
                    strcat(full_alias_cmd, execute_argv[i]);
                }
            }
            // TODO: raw mode to not auto insert newline on command enter so we can put on same line
            if (shell_config.show_full_alias_cmd) shell_print(SHELL_INFO, temp_sprintf("(%s)\n", full_alias_cmd));
        }
    }

    bool result = execute_builtin(execute_argc, execute_argv);
    if (!result) {
        result = execute_external(execute_argc, execute_argv);
    }

    if (alias_value) {
        for (int i = 0; i < execute_argc; i++) free(execute_argv[i]);
        free(execute_argv);
    }
    return result;
    return false;
}

