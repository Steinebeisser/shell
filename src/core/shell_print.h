#ifndef STEIN_SHELL_SHELL_PRINT_H
#define STEIN_SHELL_SHELL_PRINT_H

typedef enum {
    SHELL_INFO,
    SHELL_WARN,
    SHELL_ERROR,
} Shell_Print_Level;

void shell_print(Shell_Print_Level level, const char *fmt, ...);

#endif // STEIN_SHELL_SHELL_PRINT_H
