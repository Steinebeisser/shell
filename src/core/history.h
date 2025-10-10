#ifndef STEIN_SHELL_HISTORY_H
#define STEIN_SHELL_HISTORY_H

#include <stddef.h>
#include <stdbool.h>

typedef struct {
    char **lines;
    size_t max_len;
    size_t count;
    size_t start;
} History;

extern History shell_history;

void history_init();
void add_history(const char *line);
const char *get_history(size_t index); // 0 = newest
size_t history_len();
void clear_history();
void load_history();
void save_history();
bool resize_history(size_t new_size, size_t old_size); // if we at runtime set max_history_len

#endif // STEIN_SHELL_HISTORY_H
