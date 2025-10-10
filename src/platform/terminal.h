#ifndef STEIN_SHELL_PLATFORM_TERMINAL_H
#define STEIN_SHELL_PLATFORM_TERMINAL_H

#include <stdbool.h>

bool enable_raw_mode();
bool enable_ansi();
bool enable_utf8();

void disable_raw_mode();
void disable_ansi();
void disable_utf8();

#ifdef __WIN32
#include <windows.h>
HANDLE get_input_handle();
#endif

#endif // STEIN_SHELL_PLATFORM_TERMINAL_H
