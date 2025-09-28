#include "rc_parser.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "color_utils.h"
#include "config.h"
#include <limits.h>

typedef bool (*ConfigHandler)(const char *value);

typedef struct {
    const char *name;
    ConfigHandler handler;
} Options;

Options options[] = {
    {"info_color",  set_info_color},
    {"warn_color",  set_warn_color},
    {"error_color", set_error_color},
    {"prompt",      set_prompt},
};

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

char *clear_left(char *line) {
    while (*line == ' ' || *line == '\t') {
        line++;
    }
    return line;
}


bool parse_rc_line(const char *line) {
    if (!line) return false;

    char comment_char = ':';

    char *line2 = strdup(line);

    if (!line2) return false;

    line2 = clear_left(line2);

    if (*line2 == comment_char || *line2 == '\0' || *line2 == '\n') return true;

    if (strncmp(line2, "set", 3) != 0) {
        LOG_WARN("Unkown RC command %s", line2);
        return false;
    }
    line2 += 3;

    bool is_alias = false;
    line2 = clear_left(line2);
    if (*line2 == '(') {
        if (strncmp(line2, "(alias)", 7) == 0) {
            is_alias = true;
            line2 += 7;
        } else {
            LOG_WARN("Unknown Modifier: %s", line);
            return false;
        }

        line2 = clear_left(line2);
    }

    char *key_start = line2;
    while (*line2 && *line2 != '=' && *line2 != ' ' && *line2 != '\t') line2++;
    char saved = *line2;
    *line2 = '\0';
    const char *key = key_start;

    line2++;
    if (saved != '=') {
        line2 = clear_left(line2);
        if (*line2 != '=') {
            LOG_WARN("Invalid character, expected `=`, got `%c`, saved: %c", *line2, saved);
            return false;
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

    const char *value = value_start;


    LOG_DEBUG("Key: %s, Value %s", key, value);

    if (is_alias) {
        return set_alias(key, value);
    } else {
        bool handled = false;
        for (size_t i = 0; i < sizeof(options)/sizeof(options[0]); i++) {
            if (strcmp(key, options[i].name) == 0) {
                handled = options[i].handler(value);
                break;
            }
        }
        if (!handled) {
            LOG_WARN("Unknown option %s", key);
            return false;
        }
    }

    return true;
}

bool load__rc_file() {
    const char *rc_path_unresolved = "~/.shellrc";
    char path[PATH_MAX] = {0};

    if (!expand_path(rc_path_unresolved, path, PATH_MAX)) {
        LOG_WARN("Failed to expand RC path: %s", rc_path_unresolved);
        return false;
    }

    LOG_DEBUG("%s", path);
    if (path[0] == '\0') return false;

    FILE *rc_file = fopen(path, "r");
    if (!rc_file) {
        return true; // doesnt exist so we still return true
    }

    char *line = NULL;
    size_t len = 0;

    while (getline(&line, &len, rc_file) != -1) {
        LOG_DEBUG("RC LINE %s", line);
        // TODO: parse
        parse_rc_line(line);
    }

    free(line);
    fclose(rc_file);

    return true;
}
