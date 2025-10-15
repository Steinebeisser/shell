#ifndef STEIN_SHELL_CONFIG_ENV_H
#define STEIN_SHELL_CONFIG_ENV_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_ENVS 256
#define MAX_ENV_KEY_LEN 64
#define MAX_ENV_VALUE_LEN 256

typedef struct {
    char key[MAX_ENV_KEY_LEN];
    char value[MAX_ENV_VALUE_LEN];
} Env;

typedef struct {
    const char *key;
    const char *value;
    const char *description;
} EnvField;

bool set_env(const char *key, const char *value);
bool unset_env(const char *key);
const char *get_env(const char *key);

bool expand_envs(int argc, char **argv);

extern EnvField env_fields[];
extern size_t env_fields_count;

#endif // STEIN_SHELL_CONFIG_ENV_H
