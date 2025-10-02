#ifndef STEIN_SHELL_CONFIG_ENV_H
#define STEIN_SHELL_CONFIG_ENV_H

bool set__env(const char *key, const char *value);
bool unset__env(const char *key);
const char *get__env(const char *key);

bool expand_envs(int argc, char **argv);

#endif // STEIN_SHELL_CONFIG_ENV_H
