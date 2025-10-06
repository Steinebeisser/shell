#ifndef STEIN_SHELL_CONFIG_H
#define STEIN_SHELL_CONFIG_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MAX_ALIASES 128
#define MAX_ALIAS_KEY_LEN 32
#define MAX_ALIAS_VALUE_LEN 128

#define MAX_ENVS 256
#define MAX_ENV_KEY_LEN 64
#define MAX_ENV_VALUE_LEN 256

typedef struct {
    char key[MAX_ALIAS_KEY_LEN];
    char value[MAX_ALIAS_VALUE_LEN];
} Alias;

typedef struct {
    char key[MAX_ENV_KEY_LEN];
    char value[MAX_ENV_VALUE_LEN];
} Env;

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
    bool show_exit_code;
    Env envs[MAX_ENVS];
    size_t env_count;
    bool allow_env_override;
} ShellConfig;

extern ShellConfig shell_config;

bool set_info_color(const char *value);
bool unset_info_color();
const char *get_info_color();

bool set_warn_color(const char *value);
bool unset_warn_color();
const char *get_warn_color(); // TODO: color struct with rgb/int/char depending what exists

bool set_error_color(const char *value);
bool unset_error_color();
const char *get_error_color();

bool set_prompt(const char *value);
bool unset_prompt();
const char *get_prompt();

bool set_alias(const char *key, const char *value);
bool unset_alias(const char *key);
const char *get_alias(const char *key);

bool set_timeout(const char *value);
bool unset_timeout();
const char *get_timeout();

bool set_show_cmd_path(const char *value);
bool unset_show_cmd_path();
const char *get_show_cmd_path();

bool set_show_expanded_alias(const char *value);
bool unset_show_expanded_alias();
const char *get_show_expanded_alias();

bool set_show_exit_code(const char *value);
bool unset_show_exit_code();
const char *get_show_exit_code();

bool set_allow_env_override(const char *value);
bool unset_allow_env_override();
const char *get_allow_env_override();

bool set_env(const char *key, const char *value);
bool unset_env(const char *key);
const char *get_env(const char *key);

bool load_rc_file();

#endif // STEIN_SHELL_CONFIG_H
