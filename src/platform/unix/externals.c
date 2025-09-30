#if defined(__unix__)

#include "commands/externals.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include <unistd.h>
#include <sys/wait.h>

bool execute_external(int argc, char **args) {
    if (argc == 0) return false;

    pid_t pid = fork();
    if (pid < 0) {
        LOG_ERROR("Fork Failed");
        return false;
    }

    if (pid == 0) {
        execvp(args[0], args);
        LOG_ERROR("Execvp");
        _exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) && WEXITSTATUS(status) == 0;
    }
}

#endif
