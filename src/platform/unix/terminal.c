#if defined(__unix__)
#include "platform/terminal.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include <stdbool.h>
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

static struct termios orig_termios;

// https://viewsourcecode.org/snaptoken/kilo/02.enteringRawMode.html
bool enable_raw_mode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        LOG_ERROR("Failed to enable raw mode");
        return false;
    }
    atexit(disable_raw_mode);
    struct termios raw = orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        LOG_ERROR("FAiled to enable raw mode");
        return false;
    }
    return true;
}

bool enable_ansi() {
    return true;
}

bool enable_utf8() {
    return true;
}

void disable_raw_mode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1) {
        LOG_ERROR("Failed to disable raw mode");
    }
}

void disable_ansi() { }

void disable_utf8() { }
#endif // __unix__
