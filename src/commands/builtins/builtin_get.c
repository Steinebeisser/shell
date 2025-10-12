#include "commands/builtins.h"

#include "builtin_util.h"

void builtin_get(int argc, const char **argv) {
    handle_rc_command("get", argc, argv);
}
