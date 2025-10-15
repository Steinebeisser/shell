#include "env.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "third_party/nob.h"

#include "config.h"
#include "core/shell_print.h"
#include "platform/env.h"

#define ENV(key_str, value_str, description) \
    { key_str, value_str, description } ,
EnvField env_fields[] = {
    #include "env.def"
};
#undef ENV

size_t env_fields_count = NOB_ARRAY_LEN(env_fields);

int get_env_idx(const char *key) {
    for (size_t i = 0; i < shell_config.env_count; ++i) {
        if (strcmp(shell_config.envs[i].key, key) == 0) {
            return (int)i;
        }
    }

    return -1;
}

bool set_env(const char *key, const char *value) {
    if (!key || !value) return false;

    int idx = get_env_idx(key);

    if (idx >= 0) {
        if (shell_config.allow_env_override) {
            if (strlen(value) >= MAX_ENV_VALUE_LEN) {
                LOG_WARN("Value too long (> %d), will be truncated. Key: %s", MAX_ENV_VALUE_LEN, key);
                shell_print(SHELL_WARN, "Value too long (> %d), will be truncated. Key: %s\n", MAX_ENV_VALUE_LEN, key);
            }
            strncpy(shell_config.envs[idx].value, value, MAX_ENV_VALUE_LEN -1);
            shell_config.envs[idx].value[MAX_ENV_VALUE_LEN - 1] = '\0';

            setenv(key, value, 1);
            return true;
        } else {
            shell_print(SHELL_ERROR, "You cant override exisitng envs, either set `allow_env_override` to true or choose a different key, Key: %s\n", key);
            LOG_ERROR("You cant override exisitng envs, either set `allow_env_override` to true or choose a different key, Key: %s", key);
            return false;
        }
    }

    if (shell_config.env_count > MAX_ENVS) {
        shell_print(SHELL_ERROR, "You reached the env limit of %d, currently there is no way to increase without recompiling, if u want go to `src/config/config.h` and increase MAX_ENVS and recompile\n", MAX_ENVS);
        LOG_ERROR("You reached the env limit of %d, currently there is no way to increase without recompiling, if u want go to `src/config/config.h` and increase MAX_ENVS and recompile", MAX_ENVS);
        return false;
    }


    if (strlen(key) >= MAX_ENV_KEY_LEN) {
        LOG_WARN("Key too long (> %d), will be truncated. Key: %s", MAX_ENV_KEY_LEN, key);
        shell_print(SHELL_WARN, "Key too long (> %d), will be truncated. Key: %s\n", MAX_ENV_KEY_LEN, key);
    }
    if (strlen(value) >= MAX_ENV_VALUE_LEN) {
        LOG_WARN("Value too long (> %d), will be truncated. Key: %s", MAX_ENV_VALUE_LEN, key);
        shell_print(SHELL_WARN, "Value too long (> %d), will be truncated. Key: %s\n", MAX_ENV_VALUE_LEN, key);
    }

    strncpy(shell_config.envs[shell_config.env_count].key, key, MAX_ENV_KEY_LEN - 1);
    shell_config.envs[shell_config.env_count].key[MAX_ENV_KEY_LEN - 1] = '\0';

    strncpy(shell_config.envs[shell_config.env_count].value, value, MAX_ENV_VALUE_LEN - 1);
    shell_config.envs[shell_config.env_count].value[MAX_ENV_VALUE_LEN - 1] = '\0';

    if (setenv(key, value, 1) != 0) {
        shell_print(SHELL_ERROR, "Failed to set env for %s\n", key);
    }

    LOG_DEBUG("Added env with key: %s, and value %s", key, value);

    shell_config.env_count += 1;
    return true;
}

// we dont care f it exists, just say it did
bool unset_env(const char *key) {
    LOG_DEBUG("Unsetting env: %s", key);
    if (!key) return true;

    int idx = get_env_idx(key);
    LOG_DEBUG("Unsetting env: %s with index: %d", key, idx);

    if (idx < 0) return true;

    shell_config.envs[idx].value[0] = '\0';
    shell_config.envs[idx].key[0] = '\0';

    shell_config.env_count -= 1;

    unsetenv(key);

    return true;
}

const char *get_env(const char *key) {
    if (!key) return NULL;

    int idx = get_env_idx(key);

    if (idx < 0) return NULL;

    return shell_config.envs[idx].value;
}

bool expand_envs(int argc, char **argv) {
    if (!argv || argc <= 0) return false;

    for (int i = 0; i < argc; ++i) {
        if (!argv[i]) continue;

        const char *src = argv[i];
        size_t buf_len = strlen(src) + 1;
        char *dst = malloc(buf_len);
        if (!dst) return false;

        char *dst_start = dst;

        while (*src) {
            if(*src == '$') {
                src += 1;

                const char *start = src;
                while (*src && isalnum(*src))
                    src += 1;

                size_t key_len = src - start ;

                if (key_len == 0) {
                    *dst++ = '$';
                    continue;
                }

                if (key_len > MAX_ENV_KEY_LEN) {
                    free(dst_start);
                    LOG_ERROR("Env Key is too long, Max size: %d, got %llu", MAX_ENV_KEY_LEN, key_len);
                    shell_print(SHELL_ERROR, "Env Key is too long, Max size: %d, got %llu\n", MAX_ENV_KEY_LEN, key_len);
                    return false;
                }
                char key[MAX_ENV_KEY_LEN];
                memcpy(key, start, key_len);

                key[key_len] = '\0';
                const char *value = get_env(key);
                if (!value) {
                    LOG_ERROR("Failed to resolve env key, pls escape normal Dollar Signs, Key: %s", key);
                    shell_print(SHELL_ERROR, "Failed to resolve env key, pls escape normal Dollar Signs, Key: %s\n", key);
                    free(dst_start);
                    return false;
                }

                size_t value_len = strlen(value);
                size_t used_len = dst - dst_start;
                size_t needed = value_len + used_len + strlen(src) + 1;

                if (buf_len < needed) {
                    size_t offset = dst - dst_start;
                    char *tmp = realloc(dst_start, needed);
                    if (!tmp) {
                        LOG_ERROR("Failed to realloc for env expansion");
                        shell_print(SHELL_ERROR, "Failed to realloc for env expansion\n");
                        free(dst_start);
                        return false;
                    }

                    dst_start = tmp;
                    dst = dst_start + offset;
                }

                memcpy(dst, value, value_len);
                dst += value_len;
                LOG_DEBUG("Expanded %s into %s", key, value);
            } else {
                *dst++ = *src++;
            }
        }

        *dst = '\0';

        free(argv[i]);
        argv[i] = dst_start;
    }

    return true;
}

