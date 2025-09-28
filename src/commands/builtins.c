#include "builtins.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "core/shell_print.h"

int execute_builtin(int argc, char **args) {
    LOG_DEBUG("Testing if Builtin exists: %s, with %llu additional args", args[0], argc - 1);

    if (strcmp(args[0], "exit") == 0) {
        int exit_code = 0;

        if (argc > 1) {
            char *endptr;

            long value = strtol(args[1], &endptr, 10);
            if (*endptr != '\0' || value < 0 || value > 255) {
                shell_print(SHELL_ERROR, "Error: Exit Code must be a number between 0 and 255");
                return 0;
            }

            exit_code = (int)value;

            if (argc > 2) {
                LOG_DEBUG("Ignoring %d additional arguments for builtin `exit`", argc - 2);
            }
        }

        exit(exit_code);
        return 1;
    }

    return 0;
}
