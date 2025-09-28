#include "repl.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "commands/commands.h"
#include "shell_print.h"
#include "config/config.h"

void repl_loop()  {
    char *line = NULL;
    ssize_t max_arg_count = 64;
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



        char *args[max_arg_count];
        ssize_t arg_count = 0;
        char *token = strtok(line, " ");
        while (token != NULL && arg_count < max_arg_count - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        if (arg_count <= 0) {
            LOG_ERROR("repl_loop: Finished Parsing but got no tokens");
            continue;
        }

        LOG_DEBUG("Command: %s, Arg Count %llu", args[0], arg_count);

        if (execute_command(arg_count, args)) {
            LOG_DEBUG("Executed command: %s", args[0]);
            continue;
        }


        shell_print(SHELL_ERROR, "Error: Unkown or Unimplemented Command: %s\n", args[0]);
    }
}
