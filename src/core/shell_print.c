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
        case SHELL_HELP: level_color_string = temp_sprintf("\033[%dm", shell_config.help_color); break;
    }

    if (!level_color_string) {
        LOG_ERROR("shell_print: Unknown/Unimplemented Shell Level %d\n", level);
        return;
    }


    char buffer[8096]; // to be sure, dont want 2 vsnprint calls as its slow
    va_list args;
    va_start(args, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt, args);

    va_end(args);

    size_t len = strlen(buffer);
    char *crlf_buffer = malloc(len * 2 + 1);
    if (!crlf_buffer) {
        fputs(temp_sprintf("%s%s\033[0m", level_color_string, "Failed to Format Buffer"), output);
        fflush(output);
        return;
    }
    char *dst = crlf_buffer;
    for (char *src = buffer; *src; ++src) {
        if (*src == '\n' && *src - 1 != '\r') {
            *dst++ = '\r';
            *dst++ = '\n';
        } else {
            *dst++ = *src;
        }
    }
    *dst = '\0';

    fputs(temp_sprintf("%s%s\033[0m", level_color_string, crlf_buffer), output);
    free(crlf_buffer);
    fflush(output);
}
