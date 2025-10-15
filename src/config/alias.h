#ifndef STEIN_SHELL_ALIAS_H
#define STEIN_SHELL_ALIAS_H

#include <stdbool.h>
#include <stddef.h>

#define MAX_ALIASES 128
#define MAX_ALIAS_KEY_LEN 32
#define MAX_ALIAS_VALUE_LEN 128

typedef struct {
    char key[MAX_ALIAS_KEY_LEN];
    char value[MAX_ALIAS_VALUE_LEN];
} Alias;

typedef struct {
    const char *key;
    const char *value;
    const char *description;
} AliasField;

bool set_alias(const char *key, const char *value);
bool unset_alias(const char *key);
const char *get_alias(const char *key);
bool tokenize_alias_value(const char *value, char ***out_argv, int *out_argc);

extern AliasField alias_fields[];
extern size_t alias_fields_count;

#endif // STEIN_SHELL_ALIAS_H
