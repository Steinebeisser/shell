#include "shell_print.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "config/config.h"

void shell_print(Shell_Print_Level level, const char *fmt, ...) {
    char *level_color_string = NULL;
    FILE *output = stdout;

    switch (level) {
        case SHELL_INFO: level_color_string = temp_sprintf("\033[%dm", shell_config.info_color); break;
        case SHELL_WARN: level_color_string = temp_sprintf("\033[%dm", shell_config.warn_color); break;
        case SHELL_ERROR:
                level_color_string = temp_sprintf("\033[%dm", shell_config.error_color);
                output = stderr;
                break;
    }

    if (!level_color_string) {
        LOG_ERROR("shell_print: Unknown/Unimplemented Shell Level %d\n", level);
        return;
    }


    char buffer[1024];
    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt, args);

    va_end(args);

    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n' && len + 1 < sizeof(buffer)) {
        buffer[len - 1] = '\r';
        buffer[len]     = '\n';
        buffer[len + 1] = '\0';
    }

    fputs(temp_sprintf("%s%s\033[0m", level_color_string, buffer), output);
    fflush(output);
}
