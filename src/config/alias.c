#include "alias.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "config.h"
#define MAX_ALIAS_EXPANSION 10

bool tokenize_alias_value(const char *value, char ***out_argv, int *out_argc) {
    if (!value || !out_argv || !out_argc) return 0;

    *out_argv = NULL;
    *out_argc = 0;

    size_t len = strlen(value);
    if (len == 0) return true;

    int count = 0;
    bool in_word = false;
    for (size_t i = 0; i < len; ++i) {
        if (value[i] == ' ') {
            in_word = false;
        } else {
            if (!in_word) {
                in_word = true;
                count += 1;
            }
        }
    }

    char **argv = malloc((count + 1) * sizeof(char *));
    if (!argv) return false;

    int idx = 0;
    size_t start = 0;
    in_word = false;
    for (size_t i = 0; i <= len; ++i) {  // Go one past len to handle last word
        if (i == len || value[i] == ' ') {
            if (in_word) {
                size_t word_len = i - start;
                argv[idx] = malloc(word_len + 1);
                if (!argv[idx]) {
                    for (int j = 0; j < idx; ++j) free(argv[j]);
                    free(argv);
                    return false;
                }
                strncpy(argv[idx], value + start, word_len);
                argv[idx][word_len] = '\0';
                ++idx;
                in_word = false;
            }
        } else {
            if (!in_word) {
                start = i;
                in_word = true;
            }
        }
    }
    argv[idx] = NULL;
    *out_argv = argv;
    *out_argc = count;

    return true;
}

const char *get__alias(const char *key) {
    if (!key) return NULL;

    for (size_t i = 0; i < shell_config.alias_count; ++i) {
        if (strcmp(shell_config.aliases[i].key, key) == 0) {
            return shell_config.aliases[i].value;
        }
    }

    return NULL;
}

bool set__alias(const char *key, const char *value) {
    if (!key || !value) return false;

    for (size_t i = 0; i < shell_config.alias_count; ++i) {
        if (strcmp(shell_config.aliases[i].key, key) == 0) {
            strncpy(shell_config.aliases[i].value, value, MAX_ALIAS_VALUE_LEN - 1);
            shell_config.aliases[i].value[MAX_ALIAS_VALUE_LEN - 1] = '\0';
            return true;
        }
    }

    if (shell_config.alias_count >= MAX_ALIASES) return false;

    Alias *alias = &shell_config.aliases[shell_config.alias_count++];
    strncpy(alias->key, key, MAX_ALIAS_KEY_LEN - 1);
    alias->key[MAX_ALIAS_KEY_LEN - 1] = '\0';

    strncpy(alias->value, value, MAX_ALIAS_VALUE_LEN - 1);
    alias->value[MAX_ALIAS_VALUE_LEN - 1] = '\0';

    LOG_DEBUG("Set: %s, as alias for %s", key, value);

    return true;
}

// we wanna unset, dont care iif it exists or not
bool unset__alias(const char *key) {
    LOG_DEBUG("Unsetting alias %s", key);
    if (!key) return true;

    for (size_t i = 0; i < shell_config.alias_count; ++i) {
        if (strcmp(shell_config.aliases[i].key, key) == 0) {
            shell_config.aliases[i].value[0] = '\0';
            shell_config.aliases[i].key[0] = '\0';
            shell_config.alias_count -= 1;
            return true;
        }
    }

    return true;
}
