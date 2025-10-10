#include "history.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "config/config.h"
#include "config/rc_parser.h"
#include "core/repl.h"
#include "platform/getline.h"

#include <string.h>
#include <limits.h>
#include <stdlib.h>

History shell_history = {0};

void history_init() {
    if (!shell_config.enable_history) return;


    shell_history.max_len = shell_config.max_history_len;
    shell_history.count = 0;
    shell_history.start = 0;

    shell_history.lines = calloc(shell_config.max_history_len, sizeof(char*));
    if (!shell_history.lines) {
        shell_print(SHELL_ERROR, "Failed to init history: calloc failed");
        return;
    }
    for (size_t i = 0; i < shell_config.max_history_len; ++i)
        shell_history.lines[i] = calloc(MAX_LINE_LEN, sizeof(char));

    if (shell_config.enable_history_file) {
        LOG_DEBUG("Loading Hostory File");
        atexit(save_history);
        load_history();
    }
}

void add_history(const char *line) {
    if (!shell_config.enable_history || !line || strlen(line) == 0) return;

    if (shell_history.count > 1)
        if (strcmp(get_history(0), line) == 0) return;

    size_t pos;
    if (shell_history.count < shell_history.max_len) {
        pos = shell_history.count++;
    } else {
        pos = shell_history.start;
        shell_history.start = (shell_history.start + 1) % shell_history.max_len;
    }


    strncpy(shell_history.lines[pos], line, MAX_LINE_LEN - 1);
    shell_history.lines[pos][MAX_LINE_LEN - 1] = '\0';

    size_t len = strlen(shell_history.lines[pos]);
    if (len > 0 && shell_history.lines[pos][len - 1] == '\n') shell_history.lines[pos][len - 1] = '\0';
    if (len > 1 && shell_history.lines[pos][len - 2] == '\r') shell_history.lines[pos][len - 2] = '\0';
}

const char *get_history(size_t index) {
    if (index >= shell_history.count) return NULL;

    size_t real_index = (shell_history.start + shell_history.count - 1 - index) % shell_history.max_len;
    return shell_history.lines[real_index];
}

size_t history_len() {
    return shell_history.count;
}

void clear_history() {
    for (size_t i = 0; i < shell_history.max_len; ++i)
        shell_history.lines[i][0] = '\0';
    shell_history.count = 0;
    shell_history.start = 0;
}

void load_history() {
    if (!shell_config.enable_history_file) return;

    char history_path[PATH_MAX] = {0};
    if (!expand_path(shell_config.history_file, history_path, PATH_MAX)) {
        LOG_ERROR("Failed to expand history file path");
        return;
    }

    FILE *file = fopen(history_path, "r");
    if (!file) {
        LOG_ERROR("Failed to open history file for writing");
        return;
    }

    shell_history.count = 0;
    shell_history.start = 0;

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, file) != -1) {
        if (len <= 0) continue;

        if (len > 0 && line[len - 1] == '\n') line[len - 1] = '\0';
        if (len > 1 && line[len - 2] == '\r') line[len - 2] = '\0';

        add_history(line);
    }

    free(line);
    fclose(file);
}

void save_history() {
    if (!shell_config.enable_history_file) return;

    char history_path[PATH_MAX] = {0};
    if (!expand_path(shell_config.history_file, history_path, PATH_MAX)) {
        LOG_ERROR("Failed to expand history file path");
        return;
    }

    FILE *file = fopen(history_path, "w");
    if (!file) {
        LOG_ERROR("Failed to open history file for writing");
        return;
    }

    for (size_t i = 0; i < shell_history.count; ++i) {
        const char *cmd = get_history(shell_history.count - 1 - i);
        if (cmd) fprintf(file, "%s\n", cmd);
    }

    fclose(file);
}

bool resize_history(size_t new_size, size_t old_size) {
    (void) new_size;
    (void) old_size;
    // TODO:
    abort();
}
