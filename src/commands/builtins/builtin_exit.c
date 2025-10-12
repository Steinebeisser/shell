#include "commands/builtins.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "core/shell_print.h"

void builtin_exit(int argc, const char **argv) {
    int exit_code = 0;

    if (argc > 1) {
      char *endptr;

      long value = strtol(argv[1], &endptr, 10);
      if (*endptr != '\0' || value < 0 || value > 255) {
        shell_print(SHELL_ERROR, "Error: Exit Code must be a number between 0 and 255\n");
      }

      exit_code = (int)value;

      if (argc > 2) {
        LOG_DEBUG("Ignoring %d additional arguments for builtin `exit`", argc - 2);
      }
    }

    exit(exit_code);
}
