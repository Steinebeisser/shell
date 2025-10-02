#include "util.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "core/shell_print.h"

// everything that might be expanded at the end, after resolving aliases/envs, currently only think abt ~ but maybe theres more, stuff like globbing for example

bool expand_stuff(int argc, char **argv) {
    if (!argv || argc <= 0) return false;

    LOG_DEBUG("expand_stuff");
    const char *home = NULL;
#if defined(__WIN32)
    home = getenv("USERPROFILE");
#else
    home = getenv("HOME");
#endif
    if (!home) {
        LOG_WARN("expand_stuff: failed to resolve ~");
        shell_print(SHELL_WARN, "expand_stuff: failed to resolve ~");
        return false;
    }
    size_t home_len = strlen(home);

    char **new_ptrs = calloc((size_t)argc, sizeof(char *));
    if (!new_ptrs) return false;

    bool error = false;
    for (int i = 0; i < argc; ++i) {
        char *src = argv[i];
        new_ptrs[i] = NULL;

        if (!src) continue;

        LOG_DEBUG("argv: %s", src);

        bool is_tilde = (src[0] == '~' && (src[1] == '/' || src[1] == '\0'));
        size_t copy_len = is_tilde ? strlen(src + 1) : strlen(src);
        size_t new_len = is_tilde ? (home_len + copy_len + 1) : (copy_len + 1);

        char *dst = malloc(new_len);
        if (!dst) {
            LOG_WARN("expand_stuff: malloc failed for expansion\n");
            shell_print(SHELL_WARN, "expand_stuff: malloc failed for expansion\n");
            error = true;
            break;
        }

        if (is_tilde) {
            memcpy(dst, home, home_len);
            memcpy(dst + home_len, src + 1, copy_len);
        } else {
            memcpy(dst, src, copy_len);
        }
        dst[new_len - 1] = '\0';

        new_ptrs[i] = dst;
    }


    if (error) {
        for (int i = 0; i < argc; ++i) {
            free(new_ptrs[i]);
        }
        free(new_ptrs);
        return false;
    }

    for (int i = 0; i < argc; ++i) {
        free(argv[i]);
        argv[i] = new_ptrs[i];
        LOG_DEBUG("expanded argv: %s", argv[i]);
    }

    free(new_ptrs);

    return true;
}
