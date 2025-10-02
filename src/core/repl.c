#include "repl.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "commands/commands.h"
#include "shell_print.h"
#include "config/config.h"
#include "platform/getline.h"

#define MAX_ARG_COUNT 64

bool tokenize_command(const char *line, int *out_argc, char ***out_argv) {
    int argc = 0;
    char **argv = calloc(MAX_ARG_COUNT, sizeof(char *));
    if (!argv) return false;

    const char *line_start = line;
    bool ret = false;
    while (*line) {
        while (isspace(*line)) line++;
        if (!*line) {
            goto cleanup;
        }
        LOG_DEBUG("Argc: %d, first char: %c", argc, *line);

        char quote = 0;
        if (*line == '"' || *line == '\'') {
            quote = *line;
        }

        const char *arg_start = line++;
        size_t arg_len = 1;

        while (*line && ((quote && *line != quote) || (!quote && !isspace(*line)))) {
            line += 1;
            arg_len += 1;
        }
        if (quote && *line == quote) {
            line += 1;
            arg_len += 1;
        }

        LOG_DEBUG("Allocation %d Bytes for arg %d in %s", arg_len, argc, line_start);
        char *arg = strndup(arg_start, arg_len);
        if (!arg) {
            ret = false;
            goto cleanup;
        }

        if (argc >= MAX_ARG_COUNT) {
            LOG_ERROR("Reach max Arg Count, incresase MAX_ARG_COUNT in repl.c and recompile: %s", line_start);
            shell_print(SHELL_ERROR, "Reach max Arg Count, incresase MAX_ARG_COUNT in repl.c and recompile: %s\n", line_start);
            ret = false;
            goto cleanup;
        }

        argv[argc++] = arg;
    }

    argv[argc] = NULL;
    *out_argv = argv;
    *out_argc = argc;
    ret = true;

cleanup:
    if (ret == false) {
        for (int i = 0; i < argc; ++i) free(argv[i]);
        free(argv);
    }
    return ret;
}

void repl_loop()  {
    char *line = NULL;
    size_t len = 0;

    while(1) {
        // fputs(shell_promt, stdout);
        shell_print(SHELL_INFO, shell_config.prompt);

        ssize_t n = getline(&line, &len, stdin);
        if (n == -1) {
            LOG_DEBUG("repl_loop: getline failed");
            putchar('\n');
            break;
        }

        if (n > 0 && line[n-1] == '\n') line[n-1] = '\0';
        if (line[0] == '\0') continue;



        char **argv = NULL;
        int argc = 0;

        if (!tokenize_command(line, &argc, &argv)) {
            LOG_ERROR("Failed to parse command: %s", line);
            shell_print(SHELL_ERROR, "Failed to parse command");
            goto cleanup;
        }

        // char *token = strtok(line, " ");
        // while (token != NULL && arg_count < max_arg_count - 1) {
        //     args[arg_count++] = token;
        //     token = strtok(NULL, " ");
        // }
        // args[arg_count] = NULL;

        if (argc <= 0) {
            LOG_ERROR("repl_loop: Finished Parsing but got no tokens");
            goto cleanup;
        }

        LOG_DEBUG("Command: %s, Arg Count %llu", argv[0], argc);

        if (execute_command(argc, argv)) {
            LOG_DEBUG("Executed command: %s", argv[0]);
        } else {
            shell_print(SHELL_ERROR, "Error: Unkown or Unimplemented Command: %s\n", argv[0]);
        }

cleanup:
        if (argc <= 0) continue;

        for (int i = 0; i < argc; ++i) free(argv[i]);
        free(argv);
    }

    free(line);
}
