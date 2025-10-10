#include "normal_mode.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "core/shell_print.h"
#include "core/repl.h"
#include "config/config.h"
#include "core/history.h"

#define LINE_END 0
#define ERROR -1
#define OK 1

#define MOVE_CURSOR_LEFT        "\033[D"
#define MOVE_CURSOR_RIGHT       "\033[C"
#define CLEAR_RIGHT_TO_CURSOR   "\033[K"

static int history_index = -1;

// TODO: Maybe optimize if feels to slow on win laptop
static void refresh_line(char *line, int pos, int len) {
    char buf[MAX_LINE_LEN * 2];
    snprintf(buf, sizeof(buf), "\r%s%.*s%s", shell_config.prompt, len, line, CLEAR_RIGHT_TO_CURSOR);
    shell_print(SHELL_INFO, buf);
    int move_back = len - pos;
    if (move_back > 0)
        shell_print(SHELL_INFO, "\033[%dD", move_back);
}

static bool is_word_char(char c) {
    return  (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') ||
            (c >= '0' && c <= '9') ||
            (c == '_');
}

static bool move_cursor_prev_word(const char *line, int *pos) {
    if (*pos <= 0) return true;

    while (*pos > 0 && line[*pos - 1] == ' ') (*pos) -= 1;

    while (*pos > 0 && is_word_char(line[*pos - 1])) (*pos) -= 1;

    return true;
}

static bool move_cursor_next_word(const char *line, int *pos, int len) {
    if (*pos >= len) return true;

    while (*pos < len && is_word_char(line[*pos])) (*pos) += 1;

    while (*pos < len && line[*pos] == ' ') (*pos) += 1;

    return true;
}

int handle_key_normal(KeyEvent ev, char *line, int *pos, int *len) {
    switch(ev.type) {
        case KEY_UNKNOWN:
            return ERROR;

        case KEY_CHAR:
            if (*len >= MAX_LINE_LEN -1) {
                LOG_ERROR("Max line length reached");
                return ERROR;
            }
            memmove(&line[*pos + 1], &line[*pos], *len - *pos);
            line[*pos] = ev.ch;
            (*pos)++;
            (*len)++;
            refresh_line(line, *pos, *len);
            return OK;

        case KEY_ENTER:
            shell_print(SHELL_INFO, "\n");
            line[*len] = '\0';
            history_index = -1;
            return LINE_END;

        case KEY_BACKSPACE:
            if (*pos <= 0) return OK;
            memmove(&line[*pos - 1], &line[*pos], *len - *pos);
            (*pos) -= 1;
            (*len) -= 1;
            line[*len] = '\0';
            refresh_line(line, *pos, *len);
            return OK;

        case KEY_CTRL_BACKSPACE: {
            if (*pos <= 0) return OK;
            int start_pos = *pos;
            move_cursor_prev_word(line, pos);
            int delete_len = start_pos - *pos;
            memmove(&line[start_pos], &line[*pos], *len - *pos);
            *len -= delete_len;
            line[*len] = '\0';
            refresh_line(line, *pos, *len);
            return OK;
        }

        case KEY_DELETE:
            if (*pos >= *len) return OK;
            memmove(&line[*pos], &line[*pos + 1], *len - *pos - 1);
            (*len) -= 1;
            line[*len] = '\0';
            refresh_line(line, *pos, *len);
            return OK;

        case KEY_CTRL_DELETE: {
            if (*pos >= *len) return OK;
            int start_pos = *pos;
            move_cursor_next_word(line, pos, *len);
            int delete_len = *pos - start_pos;
            memmove(&line[start_pos], &line[*pos], *len - *pos + 1);
            *len -= delete_len;
            *pos = start_pos;
            line[*len] = '\0';
            refresh_line(line, *pos, *len);
            return OK;
        }

            // TODO: if process is running kill it
        case KEY_CTRL_C:
            shell_print(SHELL_WARN, " ^C\n");
            *pos = 0;
            *len = 0;
            line[0] = '\0';
            return LINE_END;

        case KEY_ARROW_LEFT:
            if (*pos <= 0) return OK;
            shell_print(SHELL_INFO, "%s", MOVE_CURSOR_LEFT);
            (*pos) -= 1;
            return OK;

        case KEY_ARROW_RIGHT:
            if (*pos >= *len) return OK;
            shell_print(SHELL_INFO, "%s", MOVE_CURSOR_RIGHT);
            (*pos) += 1;
            return OK;

        case KEY_CTRL_ARROW_LEFT:
            move_cursor_prev_word(line, pos);
            refresh_line(line, *pos, *len);
            return OK;

        case KEY_CTRL_ARROW_RIGHT:
            move_cursor_next_word(line, pos, *len);
            refresh_line(line, *pos, *len);
            return OK;

        // TODO: Hisory
        case KEY_CTRL_ARROW_UP:
        case KEY_ARROW_UP: {
            int hist_len = history_len();
            if (hist_len <= 0) return OK;

            if (history_index < hist_len - 1) {
                history_index++;
            } else {
                return OK;
            }

            LOG_DEBUG("Checking history at index %d", history_index);
            const char *last_cmd = get_history(history_index);
            if (!last_cmd) return OK;

            int cmd_len = strlen(last_cmd);
            *pos = cmd_len;
            *len = cmd_len;
            memcpy(line, last_cmd, cmd_len);
            line[cmd_len] = '\0';
            refresh_line(line, *pos, *len);
            return OK;
        }

        case KEY_CTRL_ARROW_DOWN:
        case KEY_ARROW_DOWN: {
            int hist_len = history_len();
            if (hist_len <= 0) return OK;

            if (history_index >= 0) {
                history_index--;
            } else {
                return OK;
            }

            LOG_DEBUG("Checking history at index %d", history_index);
            const char *last_cmd = (history_index >= 0) ? get_history(history_index) : "";
            int cmd_len = strlen(last_cmd);
            *pos = cmd_len;
            *len = cmd_len;
            memcpy(line, last_cmd, cmd_len);
            line[cmd_len] = '\0';
            refresh_line(line, *pos, *len);
            return OK;
        }
        default:
            return OK;
    }
}
