#include <stdio.h>

#define PGS_LOG_IMPLEMENTATION
#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "repl.h"
#include "config/config.h"


int main(int argc, char **argv) {
    if (argc > 1) {
        printf("Shell cant handle arguments atm");
        return 1;
    }

    (void)argv;

#ifdef DEBUG
    if (add_fd_output(stdout) != PGS_LOG_OK) return 1;
    LOG_DEBUG("DEBUG MODE ENABLED, ADDED STDOUT");
#else
    minimal_log_level = PGS_LOG_FATAL;
#endif
    // process_rc_file();
    load_rc_file();

    // load_plugins();

    LOG_DEBUG("STARTING REPL");

    repl_loop();

    LOG_DEBUG("Shutdown");

    // shell_cleanup()

    return 0;
}
