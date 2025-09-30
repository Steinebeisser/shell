#include "config.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include <limits.h>
#include "rc_parser.h"
#include "color_utils.h"
#include "alias.h"
#include "util.h"

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
};

bool set_info_color(const char *value) {
    return set_color("info", value);
}

bool set_warn_color(const char *value) {
    return set_color("warn", value);
}

bool set_error_color(const char *value) {
    return set_color("error", value);
}

bool set_prompt(const char *value) {
    strncpy(shell_config.prompt, value, sizeof(shell_config.prompt) - 1);
    shell_config.prompt[sizeof(shell_config.prompt) - 1] = '\0';
    return true;
}

bool set_alias(const char *key, const char *value) {
    return set__alias(key, value);
}

const char *get_alias(const char *key) {
    return get__alias(key);
}

bool set_timeout(const char *value) {
    (void)value;
    return true;
}

bool set_show_cmd_path(const char *value) {
    shell_config.show_full_cmd_path = parse_bool(value, shell_config.show_full_cmd_path);
    return true;
}

bool set_show_expanded_alias(const char *value) {
    shell_config.show_full_alias_cmd = parse_bool(value, shell_config.show_full_alias_cmd);
    return true;
}

bool set_show_exit_code(const char *value) {
    shell_config.show_exit_code = parse_bool(value, shell_config.show_exit_code);
    return true;
}

bool load_rc_file() {
    return load__rc_file();
}
