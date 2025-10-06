#ifndef STEIN_SHELL_COLOR_UTILS_H
#define STEIN_SHELL_COLOR_UTILS_H

#include <stdbool.h>

bool set_color(const char *color_type, const char *value);
bool unset_color(const char *color_type);
const char *get_color(const char *color_type);

#endif // STEIN_SHELL_COLOR_UTILS_H
