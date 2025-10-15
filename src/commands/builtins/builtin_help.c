#include "commands/builtins.h"

#include "third_party/nob.h"
#include "core/shell_print.h"
#include "config/env.h"
#include "config/config.h"

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

// TODO: improve, track unset/changed etc
static void print_envs() {
    size_t max_key_len = 0;
    for (size_t i = 0; i < shell_config.env_count; ++i) {
        size_t len = strlen(shell_config.envs[i].key);
        if (len > max_key_len) max_key_len = len;
    }

    shell_print(SHELL_INFO, "\nEnvironment Variables:\n-----------------\n");

    if (env_fields_count > 0) {
        shell_print(SHELL_INFO, "\nDefault Environment Variables:\n-----------------\n");
        for (size_t i = 0; i < env_fields_count; ++i) {
            shell_print(SHELL_INFO, "%-*s = %-20s  (%s)\n",
                        (int)max_key_len,
                        env_fields[i].key,
                        env_fields[i].value,
                        env_fields[i].description);
        }
    }

    shell_print(SHELL_INFO, "\nCustom Environment Variables:\n-----------------\n");
    for (size_t i = 0; i < shell_config.env_count; ++i) {
        bool is_default = false;
        for (size_t j = 0; j < env_fields_count; ++j) {
            if (strcmp(shell_config.envs[i].key, env_fields[j].key) == 0) {
                is_default = true;
                break;
            }
        }
        if (is_default) continue;

        shell_print(SHELL_INFO, "%-*s = %s\n",
                    (int)max_key_len,
                    shell_config.envs[i].key,
                    shell_config.envs[i].value);
    }
}

static void print_alias() {
    size_t max_key_len = 0;
    for (size_t i = 0; i < shell_config.env_count; ++i) {
        size_t len = strlen(shell_config.envs[i].key);
        if (len > max_key_len) max_key_len = len;
    }

    if (max_key_len <= 0) {
        shell_print(SHELL_WARN, "No Aliases found\n");
        return;
    }

    if (alias_fields_count > 0) {
        shell_print(SHELL_INFO, "\nDefault Aliases:\n-----------------\n");
        for (size_t i = 0; i < alias_fields_count; ++i) {
            shell_print(SHELL_INFO, "%-*s = %-20s  (%s)\n",
                        (int)max_key_len,
                        alias_fields[i].key,
                        alias_fields[i].value,
                        alias_fields[i].description);
        }
    }

    shell_print(SHELL_INFO, "\nCustom Aliases:\n-----------------\n");
    for (size_t i = 0; i < shell_config.alias_count; ++i) {
        bool is_default = false;
        for (size_t j = 0; j < alias_fields_count; ++j) {
            if (strcmp(shell_config.aliases[i].key, alias_fields[j].key) == 0) {
                is_default = true;
                break;
            }
        }
        if (is_default) continue;

        shell_print(SHELL_INFO, "%-*s = %s\n",
                    (int)max_key_len,
                    shell_config.aliases[i].key,
                    shell_config.aliases[i].value);
    }
}

void builtin_help(int argc, const char **argv) {
    if (argc == 1) {
        const Builtin *builtin = find_builtin("help");
        if (!builtin) {
            shell_print(SHELL_ERROR, "Failed to find help for help command");
            return;
        }
        print_builtin(builtin);
        return;
    }

    const char *arg = argv[1];

    if (strcmp(arg, "env") == 0) {
        print_envs();
        return;
    }

    if (strcmp(arg, "alias") == 0) {
        print_alias();
        return;
    }

    const Builtin *builtin = find_builtin(arg);
    if (!builtin) {
        shell_print(SHELL_ERROR, "Unknown help: %s\n", arg);
        const Builtin *builtin = find_builtin("help");
        if (!builtin) {
            shell_print(SHELL_ERROR, "Failed to find help for help command");
            return;
        }
        print_builtin(builtin);
        return;
    }

    print_builtin(builtin);
}
