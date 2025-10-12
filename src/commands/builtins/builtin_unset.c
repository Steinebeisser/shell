#include "commands/builtins.h"

#include "builtin_util.h"

void builtin_unset(int argc, const char **argv) {
    handle_rc_command("unset", argc, argv);
}
