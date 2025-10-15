#include "config.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "third_party/nob.h"

#include <limits.h>
#include "rc_parser.h"
#include "color_utils.h"
#include "alias.h"
#include "env.h"
#include "core/shell_print.h"
#include "platform/env.h"

ShellConfig shell_config;

#define ENV(key_str, value_str, description) \
    { .key = key_str, .value = value_str },
Env default_envs[] = {
    #include "env.def"
};
#undef ENV

#define ALIAS(key_str, value_str, description) \
    { .key = key_str, .value = value_str },
Alias default_aliases[] = {
    #include "alias.def"
};
#undef ALIAS

#define CONFIG(name, type, default_value, description, valid_options, set_func, get_func) \
    .name = default_value,
ShellConfig default_shell_config = {
    #include "config.def"
    .aliases = {},
    .alias_count = NOB_ARRAY_LEN(default_aliases),
    .envs = {},
    .env_count = NOB_ARRAY_LEN(default_envs),
};

#undef CONFIG
#define CONFIG(name, type, default_value, description, valid_options, set_func, get_func) \
    { #name, description, valid_options, set_func, get_func },
ConfigField config_fields[] = {
    #include "config.def"
};
#undef CONFIG

const size_t num_config = NOB_ARRAY_LEN(config_fields);

bool set_config_field(const char *name, const char *value) {
    if (!name || !value) return false;

#define CONFIG(config_name, type, default_value, description, valid_options, set_func, get_func) \
    if (strcmp(name, #config_name) == 0) {\
        if (set_func != NULL) return ((bool (*)(const char *))set_func)(value); \
        type out_value; \
        if (!str_to_##type(value, &out_value)) return false; \
        memcpy(&shell_config.config_name, &out_value, sizeof(type)); \
        return true; \
    }

#include "config.def"

#undef CONFIG

    return false;
}

bool str_to_int(const char *value, int *out) {
    if (!value || !out) return false;

    char *endptr;
    errno = 0;
    long val = strtol(value, &endptr, 10);

    if (errno == ERANGE || endptr == value || *endptr != '\0') {
        return false;
    }

    if (val < INT_MIN || val > INT_MAX) {
        return false;
    }

    *out = (int)val;
    return true;
}

bool str_to_string64(const char *value, string64 *out) {
    if (!value || !out) return false;

    if (strlen(value) > sizeof(string64)) {
        shell_print(SHELL_WARN, "String is to long for string 64\nGot: %s\n", value);
        return false;
    }

    strcpy(*out, value);
    return true;
}

bool str_to_int64_t(const char *value, int64_t *out) {
    if (!value || !out) return false;

    char *endptr;
    errno = 0;
    long long val = strtoll(value, &endptr, 10);

    if (errno == ERANGE || endptr == value || *endptr != '\0') {
        return false;
    }

    *out = (int64_t)val;
    return true;
}

bool str_to_bool(const char *value, bool *out) {
    if (!value || !out) return false;

    bool handled = false;
    if (strcmp(value, "1") == 0 || strcmp(value, "true") == 0) {
        handled = true;
        *out = true;
    }
    if (strcmp(value, "0") == 0 || strcmp(value, "false") == 0) {
        handled = true;
        *out = false;
    }

    return handled;
}

bool str_to_TerminalMode(const char *value, TerminalMode *out) {
    if (!value || !out) return false;

    TerminalMode mode = get_mode(value);
    if (mode == TERM_MODE_UNKOWN) return false;

    *out = mode;

    return true;
}

bool str_to_size_t(const char *value, size_t *out) {
    if (!value || !out) return false;

    char *endptr;
    errno = 0;
    size_t val = strtoull(value, &endptr, 10);

    if (errno == ERANGE || endptr == value || *endptr != '\0') {
        return false;
    }

    *out = val;

    return true;
}

bool str_to_char_ptr(const char *value, char_ptr *out) {
    if (!value || !out) return false;

    *out = strdup(value);
    if (!*out) return false;

    return true;
}


// TODO maybe have 2 macros so we dont have to do this sketchy cast, but works for now
bool unset_config_field(const char *name) {
    if (!name) return false;

#pragma GCC diagnostic ignored "-Wint-to-pointer-cast"
#define CONFIG(config_name, type, default_value, description, valid_options, set_func, get_func) \
    if (strcmp(name, #config_name) == 0) { \
        if (strcmp(#type, "char_ptr") == 0 && shell_config.config_name) free((void *)shell_config.config_name); \
        memcpy(&shell_config.config_name, &default_shell_config.config_name, sizeof(type)); \
        return true; \
    }

#include "config.def"

#undef CONFIG

    return false;
}


const char *get_config_field(const char *name) {
    if (!name) return NULL;

#define CONFIG(config_name, type, default_value, description, valid_options, set_func, get_func) \
    if (strcmp(name, #config_name) == 0) {\
        if (get_func != NULL) return ((const char* (*)(void))get_func)(); \
        if (strcmp(#type, "char_ptr") == 0) { \
            const char *value = (const char *)shell_config.config_name ? (const char *)shell_config.config_name : #default_value; \
            return value; \
        } \
        const char *value = type##_to_str(shell_config.config_name); \
        return value; \
    }

#include "config.def"

#undef CONFIG

    return NULL;
}


const char *int_to_str(int value) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%d", value);
    return buffer;
}

const char *string64_to_str(string64 value) {
    return value;
}

const char *int64_t_to_str(int64_t value) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%ld", value);
    return buffer;
}

const char *bool_to_str(bool value) {
    return value ? "true" : "false";
}

const char *TerminalMode_to_str(TerminalMode value) {
    return mode_name(value);
}

const char *size_t_to_str(size_t value) {
    static char buffer[32];
    snprintf(buffer, sizeof(buffer), "%zu", value);
    return buffer;
}

const char *char_ptr_to_str(char_ptr value) {
    return value ? value : ""; 
}

bool init_shell_config() {
    memcpy(&shell_config, &default_shell_config, sizeof(ShellConfig));

    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wstringop-overread" // if count is 0 it ommits warnings, we can ignore
    for (size_t i = 0; i < default_shell_config.env_count; i++) {
        strncpy(shell_config.envs[i].key, default_envs[i].key, MAX_ENV_KEY_LEN - 1);
        strncpy(shell_config.envs[i].value, default_envs[i].value, MAX_ENV_VALUE_LEN - 1);
        if (setenv(shell_config.envs[i].key, shell_config.envs[i].value, 1) != 0) {
            LOG_ERROR("Failed to set env %s=%s", shell_config.envs[i].key, shell_config.envs[i].value);
            shell_print(SHELL_ERROR, "Failed to set env %s\n", shell_config.envs[i].key);
            return false;
        }
    }

    for (size_t i = 0; i < default_shell_config.alias_count; i++) {
        strncpy(shell_config.aliases[i].key, default_aliases[i].key, MAX_ALIAS_KEY_LEN - 1);
        strncpy(shell_config.aliases[i].value, default_aliases[i].value, MAX_ALIAS_VALUE_LEN - 1);
    }

    #pragma GCC diagnostic pop

    LOG_DEBUG("Initialized %zu default envs", shell_config.env_count);

    return true;
}

bool load_rc_file() {
    if (!init_shell_config()) {
        return false;
    }
    return load__rc_file();
}
