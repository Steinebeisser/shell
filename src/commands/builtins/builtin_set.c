#include "commands/builtins.h"

#include "builtin_util.h"

void builtin_set(int argc, const char **argv) {
    handle_rc_command("set", argc, argv);
}
