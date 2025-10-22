#ifndef STEIN_SHELL_COLOR_UTILS_H
#define STEIN_SHELL_COLOR_UTILS_H

#include <stdbool.h>

bool set_color(const char *color_type, const char *value);

bool set_info_color(const char *value);
bool set_warn_color(const char *value);
bool set_error_color(const char *value);
bool set_help_color(const char *value);

const char *get_info_color();
const char *get_warn_color();
const char *get_error_color();
const char *get_help_color();

const char *get_color(const char *color_type);

#endif // STEIN_SHELL_COLOR_UTILS_H
