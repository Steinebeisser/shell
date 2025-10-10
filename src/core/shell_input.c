#include "shell_input.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "core/shell_print.h"
#include "input_modes/normal_mode.h"

#include <unistd.h>

#define BACKSPACE 127
#define CTRL_BACKSPACE 8
#define CTRL_C 3
#define ESCAPE 27
#define OPEN_BRACKET 91

int handle_key_input(TerminalMode mode, KeyEvent ev, char *line, int *pos, int *len) {
    switch (mode) {
        case TERM_MODE_NORMAL: return handle_key_normal(ev, line, pos, len);
        case TERM_MODE_VI:
        case TERM_MODE_EMACS:
        default:
            LOG_ERROR("Unimplemented mode: %s", mode_name(mode));
            shell_print(SHELL_ERROR, "Unimplemented mode: %s\n", mode_name(mode));
            exit(1);
            return -1;
    }
}

const char* mode_name(TerminalMode mode) {
    switch (mode) {
        case TERM_MODE_NORMAL: return "NORMAL";
        case TERM_MODE_VI:     return "VI";
        case TERM_MODE_EMACS:  return "EMACS";
        case TERM_MODE_UNKOWN: return "UNKNOWN";
        default:     return "UNKNOWN";
    }
}

TerminalMode get_mode(const char *name) {
    if (!name) return TERM_MODE_UNKOWN;

    if (strcmp(name, "normal") == 0) {
        return TERM_MODE_NORMAL;
    } else if (strcmp(name, "vi") == 0) {
        return TERM_MODE_VI;
    } else if (strcmp(name, "emacs") == 0) {
        return TERM_MODE_EMACS;
    }

    return TERM_MODE_UNKOWN;
}

KeyEvent get_key_event() {
    char c = 0;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n < 0) return (KeyEvent) {.type = KEY_UNKNOWN };

    if (c == '\n' || c == '\r') return (KeyEvent) {.type = KEY_ENTER };
    if (c == BACKSPACE) return (KeyEvent) {.type = KEY_BACKSPACE };
    if (c == CTRL_BACKSPACE) return (KeyEvent) {.type = KEY_CTRL_BACKSPACE };

    if (c == CTRL_C) return (KeyEvent) {.type = KEY_CTRL_C };

    if (c == ESCAPE) {
        // LOG_DEBUG("Escape Sequence Input");
        char seq[6] = {0};
        if (read(STDIN_FILENO, &seq[0], 1) == 0) return (KeyEvent){ .type = KEY_UNKNOWN };

        if (seq[0] == OPEN_BRACKET) {
            n = read(STDIN_FILENO, &seq[1], 5);
            int seq_len = n + 1;

            // LOG_DEBUG("Escape Sequence Length: %d", seq_len);
            // for (int i = 0; i < seq_len; ++i) {
            //     LOG_DEBUG("%d", seq[i]);
            // }
            // Arrow Keys
            if (seq_len >= 2) {
                switch (seq[1]) {
                    case 65: return (KeyEvent){ .type = KEY_ARROW_UP };
                    case 66: return (KeyEvent){ .type = KEY_ARROW_DOWN };
                    case 67: return (KeyEvent){ .type = KEY_ARROW_RIGHT };
                    case 68: return (KeyEvent){ .type = KEY_ARROW_LEFT };
                }
            }

            // del
            if (seq_len >= 3 && seq[0] == 91 && seq[1] == 51 && seq[2] == 126)
                return (KeyEvent){ .type = KEY_DELETE };

            // CTRL Arrows
            if (seq_len >= 5 && seq[0] == 91 && seq[1] == 49 && seq[2] == 59 && seq[3] == 53) {
                // LOG_DEBUG("CTRL ARROW");
                switch (seq[4]) {
                    case 68: return (KeyEvent){ .type = KEY_CTRL_ARROW_LEFT };
                    case 67: return (KeyEvent){ .type = KEY_CTRL_ARROW_RIGHT };
                    case 66: return (KeyEvent){ .type = KEY_CTRL_ARROW_DOWN };
                    case 65: return (KeyEvent){ .type = KEY_CTRL_ARROW_UP };
                }
            }

            // CTRL DELETE
            if (seq_len >= 5 && seq[0] == 91 && seq[1] == 51 && seq[2] == 59 && seq[3] == 53 && seq[4] == 126)
                return (KeyEvent){ .type = KEY_CTRL_DELETE };
        }
        return (KeyEvent){ .type = KEY_UNKNOWN };
    }

    if (c >= 32 && c <= 126) return (KeyEvent){ .type = KEY_CHAR, .ch = c };

    return (KeyEvent){ .type = KEY_UNKNOWN };
}


// Program used to get values
/*
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <termios.h>
#include <stdlib.h>

static struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

int main() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        return 1;
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
        return 1;
    }

    while(1) {
        char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        if (n <= 0) continue;

        if (c == 'q') break;

        printf("%d\r\n", c);
        fflush(stdout);
    }
    return 0;
}
*/

