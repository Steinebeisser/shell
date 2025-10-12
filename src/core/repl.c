#include "repl.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "commands/commands.h"
#include "shell_print.h"
#include "config/config.h"
#include "platform/getline.h"
#include "platform/strndup.h"
#include "core/shell_input.h"
#include "core/history.h"
#include "platform/input.h"

#include <unistd.h>

#define MAX_ARG_COUNT 64

bool tokenize_command(const char *line, int *out_argc, char ***out_argv) {
    int argc = 0;
    char **argv = calloc(MAX_ARG_COUNT, sizeof(char *));
    if (!argv) return false;

    const char *line_start = line;
    bool ret = false;
    while (*line) {
        while (isspace(*line)) line++;
        if (!*line) break;
        LOG_DEBUG("Argc: %d, first char: %c", argc, *line);

        if (*line == '=') {
            argv[argc++] = strdup("=");
            line++;
            continue;
        }

        if (*line == '(') {
            const char *arg_start = line++;
            while (*line && *line != ')') line++;
            if (*line == ')') line++;

            size_t arg_len = line - arg_start;
            char *arg = strndup(arg_start, arg_len);
            if (!arg) goto cleanup;

            argv[argc++] = arg;
            continue;
        }

        char quote = 0;
        if (*line == '"' || *line == '\'') {
            quote = *line;
        }

        const char *arg_start = line++;
        size_t arg_len = 1;

        do {
            line += 1;
            arg_len += 1;
        } while(*line && ((quote && *line != quote) || (!quote && !isspace(*line) && *line != '=' && *line != '(')));

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
    char line[MAX_LINE_LEN] = {0};
    int pos = 0;
    int len = 0;

    while(1) {
        // fputs(shell_promt, stdout);
        shell_print(SHELL_INFO, shell_config.prompt);

        while (1) {
            KeyEvent ev = get_key_event();
            LOG_DEBUG("Event: %s", type_name(ev.type));
            int res = handle_key_input(shell_config.terminal_mode, ev, line, &pos, &len);
            if (res == 0) break;
            if (res == -1) {
                LOG_ERROR("Failed to handle key input, line: %s", line);
                continue;
            }
        }

        // while (1) {
        //     char c = 0;
        //     ssize_t n = read(STDIN_FILENO, &c, 1);
        //     if (n <= 0) continue;
        //
        //     if (c == '\n' || c == '\r') {
        //         // line end
        //         shell_print(SHELL_INFO, "Entered Line\n");
        //         break;
        //     } else if (c == BACKSPACE) {
        //         // handle 
        //         shell_print(SHELL_INFO, "Backspace\n");
        //     } else if (c == CTRL_C) {
        //         shell_print(SHELL_INFO, "Killing running porcess if exists\n");
        //         shell_print(SHELL_INFO, "^C\n");
        //     } else if (c == ESCAPE) {
        //         ssize_t n = read(STDIN_FILENO, &c, 1);
        //         if (n <= 0) continue;
        //
        //         if (c == OPEN_BRACKET) {
        //             ssize_t n = read(STDIN_FILENO, &c, 1);
        //             if (n <= 0) continue;
        //
        //             if (c == 'A') {
        //                 shell_print(SHELL_INFO, "Up Arrow\n");
        //             }
        //         }
        //     } else {
        //         shell_print(SHELL_WARN, "Entered %d\n", c);
        //         line[pos++] = c;
        //     }
        //
        //     continue;
        // }

        if (pos <= 0) goto line_reset;


        char **argv = NULL;
        int argc = 0;

        add_history(line);
        //
        if (!tokenize_command(line, &argc, &argv)) {
            LOG_ERROR("Failed to parse command: %s", line);
            shell_print(SHELL_ERROR, "Failed to parse command\n");
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

        shell_print(SHELL_INFO, "\n");

cleanup:
        if (argc <= 0) continue;

        if (argv) {
            for (int i = 0; i < argc; ++i) if (argv[i]) free(argv[i]);
            if (argv) free(argv);
        }
line_reset:
        line[0] = '\0';
        pos = 0;
        len = 0;
    }

    // free(line);
}
