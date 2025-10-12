#ifndef STEIN_SHELL_BUILTINS_H
#define STEIN_SHELL_BUILTINS_H

#include <stddef.h>

typedef void (*BuiltinFunc)(int argc, const char **argv);

#define BUILTIN(name, usage, description, examples, flags, func) \
    void func(int argc, const char **argv);

#include "builtins.def"
#undef BUILTIN

typedef struct {
    const char *name;
    const char *usage;
    const char *description;
    const char **examples;
    const char **flags;
    BuiltinFunc func;
} Builtin;

int execute_builtin(int argc, const char **args);

extern Builtin builtin_table[];
extern const size_t num_builtins;

#endif // STEIN_SHELL_BUILTINS_H
