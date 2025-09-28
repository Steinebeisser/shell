#ifndef STEIN_SHELL_ALIAS_H
#define STEIN_SHELL_ALIAS_H

bool set__alias(const char *key, const char *value);
const char *get__alias(const char *key);
bool tokenize_alias_value(const char *value, char ***out_argv, int *out_argc);


#endif // STEIN_SHELL_ALIAS_H
