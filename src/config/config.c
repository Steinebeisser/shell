#include "config.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include <limits.h>
#include "rc_parser.h"
#include "color_utils.h"
#include "alias.h"
#include "util.h"
#include "env.h"
#include "core/shell_print.h"

ShellConfig shell_config = {
    .info_color = 97,
    .warn_color = 93,
    .error_color = 91,
    .prompt = "Shell > ",
    .aliases = {},
    .alias_count = 0,
    .timeout = 10000,
    .show_full_cmd_path = false,
    .show_full_alias_cmd = false,
    .show_exit_code = false,
    .envs = {
        { .key = "rc", .value = "~/.shellrc" },
    },
    .env_count = 1,
    .allow_env_override = false,
};

bool set_info_color(const char *value) {
    return set_color("info", value);
}

bool unset_info_color() {
    return unset_color("info");
}

bool set_warn_color(const char *value) {
    return set_color("warn", value);
}

bool unset_warn_color() {
    return unset_color("warn");
}

bool set_error_color(const char *value) {
    return set_color("error", value);
}

bool unset_error_color() {
    return unset_color("error");
}

const char *get_info_color() {
    return get_color("info");
}

const char *get_warn_color() {
    return get_color("warn");
}

const char *get_error_color() {
    return get_color("error");
}

bool set_prompt(const char *value) {
    strncpy(shell_config.prompt, value, sizeof(shell_config.prompt) - 1);
    shell_config.prompt[sizeof(shell_config.prompt) - 1] = '\0';
    return true;
}

bool unset_prompt() {
    shell_config.prompt[0] = '\0';
    return true;
}

const char *get_prompt() {
    return shell_config.prompt;
}

bool set_alias(const char *key, const char *value) {
    return set__alias(key, value);
}

bool unset_alias(const char *key) {
    return unset__alias(key);
}

const char *get_alias(const char *key) {
    return get__alias(key);
}

bool set_timeout(const char *value) {
    // TODO
    (void)value;
    return true;
}

bool set_show_cmd_path(const char *value) {
    shell_config.show_full_cmd_path = parse_bool(value, shell_config.show_full_cmd_path);
    return true;
}

bool unset_show_cmd_path() {
    shell_config.show_full_cmd_path = false;
    return true;
}

const char *get_show_cmd_path() {
    return shell_config.show_full_cmd_path ? "true" : "false";
}

bool set_show_expanded_alias(const char *value) {
    shell_config.show_full_alias_cmd = parse_bool(value, shell_config.show_full_alias_cmd);
    return true;
}

bool unset_show_expanded_alias() {
    shell_config.show_full_alias_cmd = false;
    return true;
}

const char *get_show_expanded_alias() {
    return shell_config.show_full_alias_cmd ? "true" : "false";
}

bool set_show_exit_code(const char *value) {
    shell_config.show_exit_code = parse_bool(value, shell_config.show_exit_code);
    return true;
}

bool unset_show_exit_code() {
    shell_config.show_exit_code = false;
    return true;
}

const char *get_show_exit_code() {
    return shell_config.show_exit_code ? "true" : "false";
}

bool set_allow_env_override(const char *value) {
    shell_config.allow_env_override = parse_bool(value, shell_config.allow_env_override);
    return true;
}

bool unset_allow_env_override() {
    shell_config.allow_env_override = false;
    return true;
}

const char *get_allow_env_override() {
    return shell_config.allow_env_override ? "true" : "false";
}

bool set_env(const char *key, const char *value) {
    return set__env(key, value);
}

bool unset_env(const char *key) {
    return unset__env(key);
}

const char *get_env(const char *key) {
    return get__env(key);
}

bool init_shell_config() {
    for (size_t i = 0; i < shell_config.env_count; i++) {
        if (setenv(shell_config.envs[i].key, shell_config.envs[i].value, 1) != 0) {
            shell_print(SHELL_ERROR, "Failed to set envs for shell config init\n");
            LOG_ERROR("Failed to set envs for shell config init");
            return false;
        }
    }
    return true;
}

bool load_rc_file() {
    if (!init_shell_config()) {
        return false;
    }
    return load__rc_file();
}
