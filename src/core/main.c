#include <stdio.h>

#define PGS_LOG_IMPLEMENTATION
#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "repl.h"
#include "config/config.h"
#include "platform/terminal.h"
#include "core/shell_print.h"
#include "core/history.h"

bool setup_in_and_output() {
    bool ret = true;
    if (!enable_raw_mode()) {
        LOG_ERROR("Failed to enable raw mode");
        ret = false;
    }
    if (!enable_ansi()) {
        LOG_ERROR("Failed to enable ANSI mode");
        ret = false;
    }
    if (!enable_utf8()) {
        LOG_ERROR("Failed to enable UTF-8");
        ret = false;
    }
    return ret;
}

void shell_cleanup() {
    disable_raw_mode();
    disable_ansi();
    disable_utf8();
}

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
    if (!setup_in_and_output()) {
        shell_print(SHELL_WARN, "Failed to setup some terminal setup, more information in log file");
    }


    // process_rc_file();
    load_rc_file();

    history_init();
    // load_plugins();

    LOG_DEBUG("STARTING REPL");

    repl_loop();

    LOG_DEBUG("Shutdown");

    shell_cleanup();

    return 0;
}
