#include "commands/builtins.h"

#include "third_party/nob.h"
#include "core/shell_print.h"

static void print_builtin(const Builtin *builtin) {
    shell_print(SHELL_INFO,
        "\nCommand: %s\n"
        "-----------------\n"
        "Usage:\n  %s\n\n"
        "Description:\n  %s\n\n",
        builtin->name, builtin->usage, builtin->description);

    if (builtin->flags && *builtin->flags) {
        shell_print(SHELL_INFO, "Flags:\n");
        for (const char **f = builtin->flags; *f != NULL; ++f) {
            shell_print(SHELL_INFO, "  %s\n", *f);
        }
        shell_print(SHELL_INFO, "\n");
    }

    if (builtin->examples && *builtin->examples) {
        shell_print(SHELL_INFO, "Examples:\n");
        for (const char **e = builtin->examples; *e != NULL; ++e) {
            shell_print(SHELL_INFO, "  %s\n", *e);
        }
        shell_print(SHELL_INFO, "\n");
    }
}

static const Builtin *find_builtin(const char *name) {
    for (size_t i = 0; i < num_builtins; ++i) {
        if (strcmp(builtin_table[i].name, name) == 0)
            return &builtin_table[i];
    }
    return NULL;
}

void builtin_help(int argc, const char **argv) {
    const Builtin *builtin = NULL;

    if (argc == 1) {
        builtin = find_builtin("help");
        if (!builtin) {
            shell_print(SHELL_ERROR, "Failed to find help for help command");
            return;
        }
    } else {
        builtin = find_builtin(argv[1]);
        if (!builtin) {
            shell_print(SHELL_ERROR, "Unknown builtin: %s\n", argv[1]);
            return;
        }
    }

    print_builtin(builtin);
}
