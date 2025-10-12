#ifndef STEIN_SHELL_BUILTINS_UTIL_H
#define STEIN_SHELL_BUILTINS_UTIL_H

#include <stdbool.h>

// returns flags, splits single dash flags into multiple, long flags are supported via double dash
// -pf gets turned into p and f
// while --persistance and --force stay long
bool get_flags(int argc, const char **argv, int *flag_argc, char ***flag_argv, int *new_argc, char ***new_argv);
char *merge_args(int start_at, int argc, const char **argv);
void handle_rc_command(const char *cmd, int argc, const char **argv);

#endif // STEIN_SHELL_BUILTINS_UTIL_H
