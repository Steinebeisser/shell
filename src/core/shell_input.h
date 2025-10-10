#ifndef STEIN_SHELL_TERMINAL_INPUT_H
#define STEIN_SHELL_TERMINAL_INPUT_H

typedef enum {
    TERM_MODE_UNKOWN,
    TERM_MODE_NORMAL,
    TERM_MODE_VI,
    TERM_MODE_EMACS
} TerminalMode;

typedef enum {
    KEY_UNKNOWN = 0,
    KEY_CHAR,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_CTRL_BACKSPACE,
    KEY_CTRL_C,
    KEY_ARROW_UP,
    KEY_CTRL_ARROW_UP,
    KEY_ARROW_DOWN,
    KEY_CTRL_ARROW_DOWN,
    KEY_ARROW_LEFT,
    KEY_CTRL_ARROW_LEFT,
    KEY_ARROW_RIGHT,
    KEY_CTRL_ARROW_RIGHT,
    KEY_DELETE,
    KEY_CTRL_DELETE,
} KeyType;

typedef struct {
    KeyType type;
    char ch;
} KeyEvent;

// returns 0 on line end, -1 on error, else 1
int handle_key_input(TerminalMode mode, KeyEvent ev, char *line, int *pos, int *len);
const char* mode_name(TerminalMode mode);
TerminalMode get_mode(const char *name);
const char *type_name(KeyType type);

#endif // STEIN_SHELL_TERMINAL_INPUT_H
