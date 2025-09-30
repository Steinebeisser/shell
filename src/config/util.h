#ifndef STEIN_SHELL_CONFIG_UTIL_H
#define STEIN_SHELL_CONFIG_UTIL_H

static bool parse_bool(const char *value, bool default_value) {
    if (!value) return default_value;

    if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0) return true;
    if (strcmp(value, "0") == 0 || strcmp(value, "false") == 0) return false;

    return default_value;
}

#endif // STEIN_SHELL_CONFIG_UTIL_H
