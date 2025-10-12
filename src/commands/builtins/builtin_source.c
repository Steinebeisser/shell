#include "commands/builtins.h"

#include "core/shell_print.h"
#include "config/rc_parser.h"

extern const char *rc_path_unresolved;

void builtin_source(int argc, const char **argv) {
    const char *filepath_unresolved = rc_path_unresolved;

    if (argc > 1) filepath_unresolved = argv[1];

    bool error_on_missing = true;
    if (!load__rc_file_path(filepath_unresolved, error_on_missing)) {
      shell_print(SHELL_WARN, "Failed to load file\n");
    }
}
