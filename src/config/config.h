#ifndef STEIN_SHELL_CONFIG_H
#define STEIN_SHELL_CONFIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_ALIASES 128
#define MAX_KEY_LEN 32
#define MAX_VALUE_LEN 128

typedef struct {
    char key[MAX_KEY_LEN];
    char value[MAX_VALUE_LEN];
} Alias;

typedef struct {
    int info_color;
    int warn_color;
    int error_color;
    char prompt[64];
    Alias aliases[MAX_ALIASES];
    size_t alias_count;
    int64_t timeout;
    bool show_full_cmd_path;
    bool show_full_alias_cmd;
} ShellConfig;

extern ShellConfig shell_config;

bool set_info_color(const char *value);
bool set_warn_color(const char *value);
bool set_error_color(const char *value);
bool set_prompt(const char *value);
bool set_alias(const char *key, const char *value);
bool set_timeout(const char *value);
bool set_show_cmd_path(const char *value);
bool set_show_expanded_alias(const char *value);
const char *get_alias(const char *key);

bool load_rc_file();

#endif // STEIN_SHELL_CONFIG_H
