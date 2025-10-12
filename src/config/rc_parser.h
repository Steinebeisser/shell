#ifndef STEIN_SHELL_RC_PARSER_H
#define STEIN_SHELL_RC_PARSER_H

#include <stdbool.h>
#include <stddef.h>

bool parse_rc_line(const char *line);
bool load__rc_file();
bool load__rc_file_path(const char *unexpanded_path, bool error_on_missing);
bool append_rc_file(const char *line, bool is_force, bool comment_old_line);
bool remove_rc_value(const char *line, bool is_force);
const char* get_rc_value(const char *line, bool insta_print);
bool expand_path(const char *unresolved_path, char *expanded_path, size_t path_len);

#endif // STEIN_SHELL_RC_PARSER_H
