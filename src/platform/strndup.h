#ifndef STEIN_SHELL_STRNDUP_H
#define STEIN_SHELL_STRNDUP_H

#if defined(__WIN32)
static inline char *strndup(const char *string, size_t len) {
    if (!string || len == 0) return NULL;

    char *dup = malloc(len + 1);
    if (!dup) return NULL;

    memcpy(dup, string, len);
    dup[len] = '\0';
    return dup;
}
#endif

#endif // STEIN_SHELL_STRNDUP_H
