#include "rc_parser.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "color_utils.h"
#include "core/shell_print.h"
#include "config.h"
#include <limits.h>
#include "platform/getline.h"

#include "third_party/nob.h"

const char *rc_path_unresolved = "~/.shellrc";

typedef bool (*ConfigHandler)(const char *value);
typedef bool (*UnsetHandler)();
typedef const char *(*GetHandler)();

typedef struct {
    const char    *name;
    ConfigHandler handler;
    UnsetHandler  unset_handler;
    GetHandler    get_handler;
} Options;

Options options[] = {
    {"info_color",           set_info_color,           unset_info_color,           get_info_color},
    {"warn_color",           set_warn_color,           unset_warn_color,           get_warn_color},
    {"error_color",          set_error_color,          unset_error_color,          get_error_color},
    {"prompt",               set_prompt,               unset_prompt,               get_prompt},
    {"show_cmd_path",        set_show_cmd_path,        unset_show_cmd_path,        get_show_cmd_path},
    {"show_expanded_alias",  set_show_expanded_alias,  unset_show_expanded_alias,  get_show_expanded_alias},
    {"show_exit_code",       set_show_exit_code,       unset_show_exit_code,       get_show_exit_code},
    {"allow_env_override",   set_allow_env_override,   unset_allow_env_override,   get_allow_env_override},
    {"timeout",              set_timeout,              unset_timeout,              get_timeout},
    {"terminal_mode",        set_terminal_mode,        unset_terminal_mode,        get_terminal_mode},
    {"enable_history_file",  set_enable_history_file,  unset_enable_history_file,  get_enable_history_file},
    {"enable_history",       set_enable_history,       unset_enable_history,       get_enable_history},
    {"max_history_len",      set_max_history_len,      unset_max_history_len,      get_max_history_len},
    {"history_file",         set_history_file,         unset_history_file,         get_history_file},
};

typedef struct {
    bool is_valid;
    bool is_set;
    bool is_unset;
    bool is_get;
    bool is_alias;
    bool is_env;
    char *key;
    char *value;
} RcLineInfo;

void free_rc_line_info(RcLineInfo *info) {
    if (info) {
        free(info->key);
        free(info->value);
        info->value =  NULL;
        info->key   =  NULL;
    }
}


char *clear_left(char *line) {
    if (!line) return NULL;
    while (*line != '\0' && *line != '\n' && (*line == ' ' || *line == '\t')) {
        line++;
    }
    return line;
}


bool parse_rc_line_info(const char *line, RcLineInfo *info) {
    memset(info, 0, sizeof(RcLineInfo));

    if (!line) return false;

    char comment_char = ':';

    char *line2 = strdup(line);
    char *line2_start = line2;

    bool ret = false;

    if (!line2) goto cleanup;

    line2 = clear_left(line2);

    if (*line2 == comment_char || *line2 == '\0' || *line2 == '\n') {
        info->is_valid = true;
        ret = true;
        goto cleanup;
    }

    if (strncmp(line2, "set", 3) == 0) {
        info->is_set = true;
        line2 += 3;
    } else if (strncmp(line2, "unset", 5) == 0) {
        info->is_unset = true;
        line2 += 5;
    } else if (strncmp(line2, "get", 3) == 0) {
        info->is_get = true;
        line2 += 3;
    } else {
        LOG_WARN("Unkown RC command %s", line2);
        ret = false;
        goto cleanup;
    }

    line2 = clear_left(line2);
    if (*line2 == '(') {
        if (strncmp(line2, "(alias)", 7) == 0) {
            info->is_alias = true;
            line2 += 7;
        } else if (strncmp(line2, "(env)", 5) == 0) {
            info->is_env = true;
            line2 += 5;
        } else {
            LOG_WARN("Unknown Modifier: %s", line);
            ret = false;
            goto cleanup;
        }

        line2 = clear_left(line2);
    }

    if (*line2 == '\0') {
        LOG_WARN("Missing key in line %s", line2);
        shell_print(SHELL_WARN, "Missing key\n");
        ret = false;
        goto cleanup;
    }

    char *key_start = line2;
    while (*line2 && *line2 != '=' && *line2 != ' ' && *line2 != '\t' && *line2 != '\n') line2++;
    char saved = *line2;
    *line2 = '\0';
    info->key = strdup(key_start);
    if (!info->key) {
        ret = false;
        goto cleanup;
    }

    if (info->is_unset) {
        ret = true;
        goto cleanup;
    }

    if (info->is_get) {
        ret = true;
        goto cleanup;
    }

    if (saved == '\0' && info->is_set) {
        LOG_WARN("Missing value for key %s", info->key);
        shell_print(SHELL_WARN, "Missing value for key: `%s`\n", info->key);
        ret = false;
        goto cleanup;
    }

    line2++;
    if (saved != '=') {
        line2 = clear_left(line2);
        if (*line2 != '=') {
            LOG_WARN("Invalid character, expected `=`, got `%c`, saved: %c", *line2, saved);
            shell_print(SHELL_WARN, "Invalid character, expected `=`, got `%c`, saved: %c\n", *line2, saved);
            ret = false;
            goto cleanup;
        }
        line2++;
    }

    line2 = clear_left(line2);

    char quote = 0;
    if (*line2 == '"' || *line2 == '\'') {
        LOG_DEBUG("QUOATE");
        quote = *line2++;
    }

    char *value_start = line2;
    char *value_end = line2;

    if (quote) {
        while(*value_end && *value_end != quote) value_end += 1;
    } else {
        while(*value_end && *value_end != '\n' && *value_end != comment_char) value_end += 1;

        char *trim_end = value_end - 1;
        while (trim_end >= value_start && (*trim_end == ' ' || *trim_end == '\t')) {
            trim_end -= 1;
        }
        value_end = trim_end + 1;
    }

    *value_end = '\0';

    info->value = strdup(value_start);
    if (!info->value) {
        ret = false;
        goto cleanup;
    }

    LOG_DEBUG("Key: %s, Value %s", info->key, info->value);

    ret = true;

cleanup:
    if (line2_start) free(line2_start);
    info->is_valid = ret && (info->is_set || info->is_unset);
    return ret;
}

bool expand_path(const char *unresolved_path, char *expanded_path, size_t path_len) {
    if (!unresolved_path || !expanded_path || path_len == 0) return false;

    size_t remaining = path_len - 1;

    const char *src = unresolved_path;
    char *dst = expanded_path;

    while(*src && remaining > 0) {
        switch (*src) {
            case '~': {
                const char *home = NULL;
#if defined(__unix__)
                home = getenv("HOME");
#elif defined(__WIN32)
                home = getenv("USERPROFILE");
#endif

                if (!home) {
                    LOG_WARN("expand_path: Failed to resolve `~` in path expansion");
                    return false;
                }

                size_t home_len = strlen(home);
                if (home_len > remaining) {
                    LOG_WARN("expand_path: Path is too long");
                    return false;
                }

                memcpy(dst, home, home_len);
                dst += home_len;
                remaining -= home_len;
                src += 1;
                break;
            }
            default: {
              *dst++ = *src++;
              remaining -= 1;
            }
        }
    }

    *dst = '\0';

    return true;
}


bool parse_rc_line(const char *line) {
    if (!line) return false;

    RcLineInfo info = {0};
    if (!parse_rc_line_info(line, &info)) {
        free_rc_line_info(&info);
        return false;
    }

    if (! (info.is_set || info.is_unset) ) { // comments/empty lines ok
        free_rc_line_info(&info);
        return true;
    }

    bool ret = false;

    if (info.is_unset) {
        if (info.is_env) {
            ret = unset_env(info.key);
        } else if (info.is_alias) {
            ret = unset_alias(info.key);
        } else {
            bool handled = false;
            for (size_t i = 0; i < NOB_ARRAY_LEN(options); i++) {
                if (strcmp(info.key, options[i].name) == 0) {
                    handled = options[i].unset_handler();
                    break;
                }
            }
            if (!handled) {
                LOG_WARN("Unknown option %s", info.key);
                ret = false;
            } else {
                ret = true;
            }
        }
    } else if (info.is_set) {
        if (info.is_env) {
            ret = set_env(info.key, info.value);
        } else if (info.is_alias) {
            ret = set_alias(info.key, info.value);
        } else {
            bool handled = false;
            for (size_t i = 0; i < NOB_ARRAY_LEN(options); i++) {
                if (strcmp(info.key, options[i].name) == 0) {
                    handled = options[i].handler(info.value);
                    break;
                }
            }
            if (!handled) {
                LOG_WARN("Failed to Set Value for key: `%s`", info.key);
                shell_print(SHELL_WARN, "Failed to Set Value for key: `%s`\n", info.key);
                ret = false;
            } else {
                ret = true;
            }
        }
    } else {
        LOG_ERROR("Unkown Rc line value, %s", line);
        shell_print(SHELL_ERROR, "Unkown Rc line value\n");
        ret = false;
    }

    free_rc_line_info(&info);
    return ret;
}

bool load__rc_file() {
    char path[PATH_MAX] = {0};

    if (!expand_path(rc_path_unresolved, path, PATH_MAX)) {
        LOG_WARN("Failed to expand RC path: %s", rc_path_unresolved);
        return false;
    }

    LOG_DEBUG("Using %s as rc path", path);
    if (path[0] == '\0') return false;

    FILE *rc_file = fopen(path, "r");
    if (!rc_file) {
        LOG_DEBUG("Rc File doesnt exist or cant be opened");
        return true; // doesnt exist so we still return true
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, rc_file) != -1) {
        LOG_DEBUG("RC LINE %s", line);
        // TODO: parse
        parse_rc_line(line);
    }
    LOG_DEBUG("Finished reading rc file");

    free(line);
    fclose(rc_file);

    return true;
}

char *trim_newline(const char *line) {
    size_t len = strlen(line);
    if (len == 0) return NULL;
    char *copy = strdup(line);
    if (len > 1 && copy[len - 1] == '\n') {
        if (len > 2 && copy[len - 2] == '\r') {
            copy[len - 2] = '\0';
        }
        copy[len - 1] = '\0';
    }
    return copy;
}

typedef struct {
    char **items;
    size_t count;
    size_t capacity;
} Lines;

bool edit_rc_file(const char *line, bool add, bool is_force, bool comment_old) {
    // asm("int3");
    LOG_DEBUG("-----\n%s\n-----", line);
    LOG_DEBUG("Original line pointer: %p, content: '%s'", (void*)line, line);
    char path[PATH_MAX] = {0};

    if (!expand_path(rc_path_unresolved, path, PATH_MAX)) {
        LOG_WARN("Failed to expand RC path: %s", rc_path_unresolved);
        return false;
    }

    LOG_DEBUG("%s", path);
    if (path[0] == '\0') return false;

    RcLineInfo info = {0};
    if (!parse_rc_line_info(line, &info)) {
        LOG_WARN("Failed to parse line for edit: %s", line);
        free_rc_line_info(&info);
        return false;
    }

    if (!info.is_valid) {
        free_rc_line_info(&info);
        return false;
    }

    if (add && info.is_env && !shell_config.allow_env_override && !is_force) {
        shell_print(SHELL_ERROR, "allow_env_override is set to false, either provide -f/--force flag or change allow_env_override to true\n");
        LOG_ERROR("allow_env_override is set to false, either provide -f/--force flag or change allow_env_override to true, %s", line);
        free_rc_line_info(&info);
        return false;
    }

    FILE *rc_read = fopen(path, "r");
    if (!rc_read) {
        if (add) {
            FILE *rc_write = fopen(path, "w");
            if (!rc_write) return false;
            fprintf(rc_write, "%s\n", line);
            fclose(rc_write);
        }
        free_rc_line_info(&info);
        return true;
    }

    Lines old_lines = {0};

    char *rc_line = NULL;
    size_t rc_len = 0;
    while (getline(&rc_line, &rc_len, rc_read) != -1) {
        char *dup = strdup(rc_line);

        if (!dup) goto cleanup;

        nob_da_append(&old_lines, dup);
    }
    free(rc_line);
    fclose(rc_read);

    Lines new_lines = {0};

    bool overwrote_value = false;
    for (size_t i = 0; i < old_lines.count; ++i) {
        RcLineInfo old_info = {0};
        bool ok = parse_rc_line_info(old_lines.items[i], &old_info);
        if (!ok || !old_info.is_valid) {
            char *dup = strdup(old_lines.items[i]);
            if (!dup) {
                free_rc_line_info(&old_info);
                continue;
            }
            nob_da_append(&new_lines, dup);
            free_rc_line_info(&old_info);
            continue;
        }

        if ((old_info.is_set || old_info.is_unset) && old_info.is_alias == info.is_alias && old_info.is_env == info.is_env && strcmp(old_info.key, info.key) == 0) {
            if (comment_old && add) {
                char *trimmed = trim_newline(old_lines.items[i]);
                if (!trimmed) {
                    free_rc_line_info(&old_info);
                    continue;
                }
                char *commented = strdup(temp_sprintf(": %s : commented via -c/-comment_old flag\n", trimmed));
                free(trimmed);
                if (!commented) {
                    free_rc_line_info(&old_info);
                    continue;
                }
                nob_da_append(&new_lines, commented);
            }
            free_rc_line_info(&old_info);
            continue;
        }

        char *dup = strdup(old_lines.items[i]);
        if (!dup) {
            free_rc_line_info(&old_info);
            continue;
        }

        nob_da_append(&new_lines, dup);
        free_rc_line_info(&old_info);
    }

    if (!overwrote_value) {
        char *newline = strdup(temp_sprintf("\n%s", line)); // its only importatnt that its on a new line, not that it ends on one
        LOG_DEBUG(line);
        if (!newline) goto cleanup;
        nob_da_append(&new_lines, newline);
    }

    char tmp_path[PATH_MAX] = {0};
    char *unresolved_tmp_path = "~/.shellrc.tmp";
    if (!expand_path(unresolved_tmp_path, tmp_path, PATH_MAX)) {
        LOG_WARN("Failed to expand tmp RC path: %s", unresolved_tmp_path);
        goto cleanup;
    }

    FILE *tmp_file = fopen(tmp_path, "w");
    if (!tmp_file) {
        LOG_WARN("Failed to open tmp RC path: %s", unresolved_tmp_path);
        goto cleanup;
    }

    for (size_t i = 0; i < new_lines.count; ++i) {
        fprintf(tmp_file, "%s", new_lines.items[i]);
    }

    fclose(tmp_file);

#if defined(__WIN32)
    size_t tmp_path_len = strlen(tmp_path);
    char tmp2_windows_file[PATH_MAX] = {0};
    memcpy(tmp2_windows_file, tmp_path, tmp_path_len);
    tmp2_windows_file[tmp_path_len] = '\0';
    tmp2_windows_file[tmp_path_len - 1] = 'd';
    tmp2_windows_file[tmp_path_len - 2] = 'l';
    tmp2_windows_file[tmp_path_len - 3] = 'o';
    if (_access(tmp2_windows_file, 0) == 0) {
        if (remove(tmp2_windows_file) != 0) {
            LOG_ERROR("Failed to delete old file, hate microsoft, not me");
            goto cleanup;
        }
    }
    if (rename(path, tmp2_windows_file) != 0) {
        LOG_ERROR("Failed to rename original to temp file %s to %s", path, tmp2_windows_file);
        goto cleanup;
    }
#endif

    if (rename(tmp_path, path) != 0) {
        LOG_ERROR("Failed to rename temp file to %s", path);
#if defined(__WIN32)
    if (rename(tmp2_windows_file, path) != 0) {
        LOG_ERROR("Failed to rename new file to original name, but also failed to restore old file, rename %s manally to %s", tmp2_windows_file, path);
        goto cleanup;
    }
#endif
        remove(tmp_path);
        goto cleanup;
    }

    for (size_t i = 0; i < old_lines.count; ++i) free(old_lines.items[i]);
    free(old_lines.items);
    for (size_t i = 0; i < new_lines.count; ++i) free(new_lines.items[i]);
    free(new_lines.items);
    free_rc_line_info(&info);
    return true;

cleanup:
    for (size_t i = 0; i < old_lines.count; ++i) free(old_lines.items[i]);
    free(old_lines.items);
    for (size_t i = 0; i < new_lines.count; ++i) free(new_lines.items[i]);
    free(new_lines.items);
    free_rc_line_info(&info);
    return false;
}

bool append_rc_file(const char *line, bool is_force, bool comment_old_lines) {
    bool is_add = true;
    return edit_rc_file(line, is_add, is_force, comment_old_lines);
}

bool remove_rc_value(const char *line, bool is_force) {
    bool is_add = false;
    bool comment_old = false;
    return edit_rc_file(line, is_add, is_force, comment_old);
}

const char* get_rc_value(const char *line, bool insta_print) {
    if (!line) return NULL;
    LOG_DEBUG("Getting Rc Value: %s", line);

    RcLineInfo info = {0};
    if (!parse_rc_line_info(line, &info)) {
        LOG_ERROR("Failed to parse rc line: %s", line);
        shell_print(SHELL_ERROR, "Failed to parse rc line");
        free_rc_line_info(&info);
        return NULL;
    }

    if (!info.is_get) {
        free_rc_line_info(&info);
        return NULL;
    }

    const char *result = NULL;

    if (info.is_alias) {
        result = get_alias(info.key);
    } else if (info.is_env) {
        result = get_env(info.key);
    } else {
        for (size_t i = 0; i < NOB_ARRAY_LEN(options); i++) {
            if (strcmp(info.key, options[i].name) == 0) {
                result = options[i].get_handler();
                break;
            }
        }
    }

    if (insta_print) {
        if (result) {
            if (info.is_alias) {
                shell_print(SHELL_INFO, "(Alias)%s: %s\n", info.key, result);
            } else if (info.is_env) {
                shell_print(SHELL_INFO, "(Env)%s: %s\n", info.key, result);
            } else {
                shell_print(SHELL_INFO, "%s: %s\n", info.key, result);
            }
        } else {
            shell_print(SHELL_ERROR, "Unknown Key: %s\n", info.key);
        }
    }

    free_rc_line_info(&info);
    return result;
}
