#ifdef __WIN32
#include "platform/terminal.h"

#include <windows.h>

static bool ansi_mode_enabled = false;
static HANDLE hOut = INVALID_HANDLE_VALUE;
static DWORD original_output_mode = 0;

static UINT g_oldOutputCP = 0;
static UINT g_oldInputCP = 0;

bool raw_mode_enabled = false;
static DWORD original_input_mode = 0;
static HANDLE hIn = INVALID_HANDLE_VALUE;

HANDLE get_input_handle() {
    return hIn;
}

bool enable_raw_mode() {
    if (raw_mode_enabled) {
        return true;
    }

    SetConsoleCtrlHandler(NULL, TRUE);

    hIn = GetStdHandle(STD_INPUT_HANDLE);
    if (!GetConsoleMode(hIn, &original_input_mode)) {
        return false;
    }
    DWORD mode = original_input_mode;

    mode &= ~(ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT | ENABLE_PROCESSED_INPUT);
    if (!SetConsoleMode(hIn, mode)) {
        return false;
    }

    atexit(disable_raw_mode);
    raw_mode_enabled = true;
    return true;
}

bool enable_ansi() {
    if (ansi_mode_enabled) {
        return true;
    }

    hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) {
        return false;
    }

    if (!GetConsoleMode(hOut, &original_output_mode)) {
        return false;
    }
    DWORD dwMode = original_output_mode;

    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING | DISABLE_NEWLINE_AUTO_RETURN | ENABLE_PROCESSED_OUTPUT;
    if (!SetConsoleMode(hOut, dwMode)) {
        return false;
    }

    ansi_mode_enabled = true;
    atexit(disable_ansi);
    return true;
}

bool enable_utf8() {
    g_oldOutputCP = GetConsoleOutputCP();
    g_oldInputCP = GetConsoleCP();

    if (!SetConsoleOutputCP(65001)) return false;
    if (!SetConsoleCP(65001)) return false;
    atexit(disable_utf8);
    return true;
}

void disable_raw_mode() {
    if (!raw_mode_enabled || original_input_mode == 0 || hIn == INVALID_HANDLE_VALUE) {
        return;
    }

    FlushConsoleInputBuffer(hIn);
    SetConsoleMode(hIn, original_input_mode);
    raw_mode_enabled = false;
}

void disable_ansi() {
    if (!ansi_mode_enabled || original_output_mode == 0 || hOut == INVALID_HANDLE_VALUE) {
        return;
    }

    SetConsoleMode(hOut, original_output_mode);
    ansi_mode_enabled = false;
}
void disable_utf8() {
    if (g_oldOutputCP != 0) {
        SetConsoleOutputCP(g_oldOutputCP);
    }
    if (g_oldInputCP != 0) {
        SetConsoleCP(g_oldInputCP);
    }
}

#endif // __WIN32
