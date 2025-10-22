#ifndef STEIN_SHELL_CONFIG_H
#define STEIN_SHELL_CONFIG_H

#include "core/shell_input.h"
#include "config/env.h"
#include "config/alias.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>


#define CONFIG(name, type, default_value, description, valid_options, set_func, get_func, aliases) \
    type name;
typedef char string64[64];
typedef char* char_ptr;
typedef struct {
    #include "config.def"
    Alias aliases[MAX_ALIASES];
    size_t alias_count;
    Env envs[MAX_ENVS];
    size_t env_count;
} ShellConfig;

#undef CONFIG

typedef struct {
    const char *name;
    const char *description;
    const char **valid_options;
    bool (*set)(const char *value);
    const char *(*get)();
    const char **aliases;
} ConfigField;


extern ConfigField config_fields[];
extern const size_t num_config;

extern ShellConfig shell_config;

bool set_config_field(const char *name, const char *value);
bool unset_config_field(const char *name);
const char *get_config_field(const char *name);
const char *get_default_config_field(const char *name);

bool str_to_int(const char *value, int *out);
bool str_to_string64(const char *value, string64 *out);
bool str_to_int64_t(const char *value, int64_t *out);
bool str_to_bool(const char *value, bool *out);
bool str_to_TerminalMode(const char *value, TerminalMode *out);
bool str_to_size_t(const char *value, size_t *out);
bool str_to_char_ptr(const char *value, char_ptr *out);

const char *int_to_str(int value);
const char *string64_to_str(string64 value);
const char *int64_t_to_str(int64_t value);
const char *bool_to_str(bool value);
const char *TerminalMode_to_str(TerminalMode value);
const char *size_t_to_str(size_t value);
const char *char_ptr_to_str(char_ptr value);

bool load_rc_file();

#endif // STEIN_SHELL_CONFIG_H
