#if defined(__unix__)

#include "commands/externals.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "config/config.h"
#include "core/shell_print.h"
#include "platform/terminal.h"

#include <unistd.h>
#include <sys/wait.h>
#include <limits.h>

bool executable_exists(const char *name, char **out_path) {
    if (!name || name[0] == '\0') return false;
    if (strchr(name, '/')) {
        if(access(name, X_OK) == 0) {
            *out_path = strdup(name);
            return true;
        }
        return false;
    }
    const char *path = getenv("PATH");
    if (!path) path = "/bin:/usr/bin";
    char *paths = strdup(path);
    char *save = paths;
    char *dir;
    while ((dir = strsep(&paths, ":")) != NULL) {
        if (*dir == '\0') dir = ".";
        size_t n = strlen(dir) + 1 + strlen(name) + 1;
        char *candidate = malloc(n);
        snprintf(candidate, n, "%s/%s", dir, name);
        if (access(candidate, X_OK) == 0) {
            free(save);
            *out_path = candidate;
            return true;
        }
        free(candidate);
    }
    free(save);
    return false;
}

// TODO: waitpid with timeout: https://exceptional-code.blogspot.com/2013/04/waiting-for-child-process-with-timeout.html

bool execute_external(int argc, char **argv) {
    if (argc == 0) return false;
    char *resolved_path = NULL;
    if (!executable_exists(argv[0], &resolved_path)) return false;

    bool ret = false;

    disable_raw_mode();
    pid_t pid = fork();
    if (pid < 0) {
        LOG_ERROR("Fork Failed");
        ret = false;
        goto cleanup;
    }

    if (pid == 0) {
        if (shell_config.show_cmd_path) {
            shell_print(SHELL_INFO, "%s\n", resolved_path);
            free(resolved_path);
        }
        execv(resolved_path, argv);
        LOG_ERROR("execv(%s) failed: %s", argv[0], strerror(errno));
        _exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if (shell_config.show_exit_code) {
            shell_print(SHELL_INFO, "%s exited with Exit Code: %d\n", argv[0], WEXITSTATUS(status));
        }
        ret = true;
    }

cleanup:
    enable_raw_mode();
    if (resolved_path) free(resolved_path);
    return ret;
}

#endif
