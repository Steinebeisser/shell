#include "shell_input.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "core/shell_print.h"
#include "input_modes/normal_mode.h"

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

const char *type_name(KeyType type) {
    switch (type) {
        case KEY_UNKNOWN: return "UNKNOWN";
        case KEY_CHAR: return "CHAR";
        case KEY_ENTER: return "ENTER";
        case KEY_BACKSPACE: return "BACKSPACE";
        case KEY_CTRL_BACKSPACE: return "CTRL_BACKSPACE";
        case KEY_CTRL_C: return "CTRL_C";
        case KEY_ARROW_UP: return "ARROW_UP";
        case KEY_CTRL_ARROW_UP: return "CTRL_ARROW_UP";
        case KEY_ARROW_DOWN: return "ARROW_DOWN";
        case KEY_CTRL_ARROW_DOWN: return "CTRL_ARROW_DOWN";
        case KEY_ARROW_LEFT: return "ARROW_LEFT";
        case KEY_CTRL_ARROW_LEFT: return "CTRL_ARROW_LEFT";
        case KEY_ARROW_RIGHT: return "ARROW_RIGHT";
        case KEY_CTRL_ARROW_RIGHT: return "CTRL_ARROW_RIGHT";
        case KEY_DELETE: return "DELETE";
        case KEY_CTRL_DELETE: return "CTRL_DELETE";
        default: return "Unkown";
    }
}
