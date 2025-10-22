#include "commands/builtins.h"

#include "third_party/nob.h"
#include "core/shell_print.h"
#include "config/env.h"
#include "config/config.h"

static void print_builtin(const Builtin *builtin) {
    shell_print(SHELL_HELP,
        "\nCommand: %s\n"
        "-----------------\n"
        "Usage:\n  %s\n\n"
        "Description:\n  %s\n\n",
        builtin->name, builtin->usage, builtin->description);

    if (builtin->flags && *builtin->flags) {
        shell_print(SHELL_HELP, "Flags:\n");
        for (const char **f = builtin->flags; *f != NULL; ++f) {
            shell_print(SHELL_HELP, "  %s\n", *f);
        }
        shell_print(SHELL_HELP, "\n");
    }

    if (builtin->examples && *builtin->examples) {
        shell_print(SHELL_HELP, "Examples:\n");
        for (const char **e = builtin->examples; *e != NULL; ++e) {
            shell_print(SHELL_HELP, "  %s\n", *e);
        }
        shell_print(SHELL_HELP, "\n");
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

    shell_print(SHELL_HELP, "\nEnvironment Variables:\n-----------------\n");

    if (env_fields_count > 0) {
        shell_print(SHELL_HELP, "\nDefault Environment Variables:\n-----------------\n");
        for (size_t i = 0; i < env_fields_count; ++i) {
            shell_print(SHELL_HELP, "%-*s = %-20s  (%s)\n",
                        (int)max_key_len,
                        env_fields[i].key,
                        env_fields[i].value,
                        env_fields[i].description);
        }
    }

    shell_print(SHELL_HELP, "\nCustom Environment Variables:\n-----------------\n");
    for (size_t i = 0; i < shell_config.env_count; ++i) {
        bool is_default = false;
        for (size_t j = 0; j < env_fields_count; ++j) {
            if (strcmp(shell_config.envs[i].key, env_fields[j].key) == 0) {
                is_default = true;
                break;
            }
        }
        if (is_default) continue;

        shell_print(SHELL_HELP, "%-*s = %s\n",
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
        shell_print(SHELL_HELP, "\nDefault Aliases:\n-----------------\n");
        for (size_t i = 0; i < alias_fields_count; ++i) {
            shell_print(SHELL_HELP, "%-*s = %-20s  (%s)\n",
                        (int)max_key_len,
                        alias_fields[i].key,
                        alias_fields[i].value,
                        alias_fields[i].description);
        }
    }

    shell_print(SHELL_HELP, "\nCustom Aliases:\n-----------------\n");
    for (size_t i = 0; i < shell_config.alias_count; ++i) {
        bool is_default = false;
        for (size_t j = 0; j < alias_fields_count; ++j) {
            if (strcmp(shell_config.aliases[i].key, alias_fields[j].key) == 0) {
                is_default = true;
                break;
            }
        }
        if (is_default) continue;

        shell_print(SHELL_HELP, "%-*s = %s\n",
                    (int)max_key_len,
                    shell_config.aliases[i].key,
                    shell_config.aliases[i].value);
    }
}

size_t find_matching_configs(const char *arg, const ConfigField **out_matches, size_t max_matches) {
    size_t count = 0;
    for (size_t i = 0; i < num_config && count < max_matches; ++i) {
        const ConfigField *field = &config_fields[i];
        if (strcmp(field->name, arg) == 0) {
            out_matches[count++] = field;
            continue;
        }
        if (field->aliases) {
            for (const char **alias = field->aliases; *alias != NULL; ++alias) {
                if (strcmp(*alias, arg) == 0) {
                    out_matches[count++] = field;
                    break;
                }
            }
        }
    }
    return count;
}

static void print_config(const char *arg, const ConfigField *field, size_t index) {
    if (index == 0) {
        shell_print(SHELL_HELP, "\nMatching Configs for '%s':\n-----------------\n", arg);
    }
    shell_print(SHELL_HELP,
        "\nConfig %zu: %s\n"
        "-----------------\n"
        "Description:\n  %s\n\n",
        index + 1, field->name, field->description);

    const char *default_value = get_default_config_field(field->name);
    const char *current_value = get_config_field(field->name);

    if (current_value) shell_print(SHELL_HELP, "Current value: %s\n", current_value);
    if (default_value) shell_print(SHELL_HELP, "Default value: %s\n", default_value);
    if (current_value || default_value) shell_print(SHELL_HELP, "\n");

    if (field->valid_options && *field->valid_options) {
        shell_print(SHELL_HELP, "Valid options:\n");
        for (const char **option = field->valid_options; *option != NULL; ++option) {
            shell_print(SHELL_HELP, "  %s\n", *option);
        }
        shell_print(SHELL_HELP, "\n");
    }

    if (field->aliases && *field->aliases) {
        shell_print(SHELL_HELP, "Help Aliases:\n");
        for (const char **alias = field->aliases; *alias != NULL; ++alias) {
            shell_print(SHELL_HELP, "  %s\n", *alias);
        }
        shell_print(SHELL_HELP, "\n");
    }

    shell_print(SHELL_HELP, "Usage:\n  set %s <value>\n\n", field->name);
}

// TODO: make good looking
void print_all_config() {
    shell_print(SHELL_HELP, "\nAvailable Config Fields:\n-----------------------\n");
    for (size_t i = 0; i < num_config; ++i) {
        const ConfigField *field = &config_fields[i];
        shell_print(SHELL_HELP, "%s", field->name);

        if (field->aliases && *field->aliases) {
            shell_print(SHELL_HELP, " (aliases: ");
            for (const char **alias = field->aliases; *alias != NULL; ++alias) {
                shell_print(SHELL_HELP, "%s", *alias);
                if (*(alias + 1) != NULL) shell_print(SHELL_HELP, ", ");
            }
            shell_print(SHELL_HELP, ")");
        }
        shell_print(SHELL_HELP, "\n");
    }
    shell_print(SHELL_HELP, "\n");
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

    if (strcmp(arg, "config") == 0) {
        print_all_config();
        return;
    }

    const ConfigField *matches[32];
    size_t config_matches = find_matching_configs(arg, matches, 32);
    if (config_matches > 0) {
        for (size_t i = 0; i < config_matches; ++i) {
            print_config(arg, matches[i], i);
        }
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
