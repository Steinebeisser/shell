#ifndef STEIN_SHELL_PLATFORM_ENV_H
#define STEIN_SHELL_PLATFORM_ENV_H

#if defined(__WIN32)
static inline int setenv(const char *key, const char *value, int override) {
    if (!override) {
        char *existing = getenv(key);
        if (existing) return 0;
    }
    return _putenv_s(key, value);
}

static inline int unsetenv(const char *key) {
    return _putenv_s(key, "");
}
#endif

#endif // STEIN_SHELL_PLATFORM_ENV_H
