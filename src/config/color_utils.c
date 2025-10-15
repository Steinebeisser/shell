#include "color_utils.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "config.h"
#include "core/shell_print.h"

typedef struct {
    const char *name;
    int code;
} ColorEntry;

static ColorEntry color_table[] = {
    {"black",   30},
    {"red",     31},
    {"green",   32},
    {"yellow",  33},
    {"blue",    34},
    {"magenta", 35},
    {"cyan",    36},
    {"white",   37},
    {"bright_black",   90},
    {"bright_red",     91},
    {"bright_green",   92},
    {"bright_yellow",  93},
    {"bright_blue",    94},
    {"bright_magenta", 95},
    {"bright_cyan",    96},
    {"bright_white",   97},
};

int parse_color(const char *name) {
    if (!name) return -1;

    for (size_t i = 0; i < sizeof(color_table)/sizeof(color_table[0]); i++) {
        if (strcmp(name, color_table[i].name) == 0)
            return color_table[i].code;
    }

    if (isdigit((unsigned char)name[0])) {
        int code = atoi(name);
        if (code >= 30 && code <= 97) return code;
    }

    LOG_WARN("Unknown color name: %s", name);
    shell_print(SHELL_WARN, "Unknown color name: %s\n", name);
    return -1;
}

bool set_color(const char *color_type, const char *value) {
    int color_value = parse_color(value);
    if (color_value == -1)
        return false;

    if (strcmp(color_type, "info") == 0) {
        shell_config.info_color = color_value;
    } else if (strcmp(color_type, "warn") == 0) {
        shell_config.warn_color = color_value;
    } else if (strcmp(color_type, "error") == 0) {
        shell_config.error_color = color_value;
    } else {
        LOG_WARN("Unknown color type passed: %s", color_type);
        shell_print(SHELL_WARN, "Unknown color type passed: %s\n", color_type);
        return false;
    }

    return true;
}

bool unset_color(const char *color_type) {
    int white = 97;

    if (strcmp(color_type, "info") == 0) {
        shell_config.info_color = white;
    } else if (strcmp(color_type, "warn") == 0) {
        shell_config.warn_color = white;
    } else if (strcmp(color_type, "error") == 0) {
        shell_config.error_color = white;
    } else {
        LOG_WARN("Unknown color type passed: %s", color_type);
        return false;
    }

    return true;
}


const char *get_color(const char *color_type) {
    int code;

    if (strcmp(color_type, "info") == 0) {
        code = shell_config.info_color;
    } else if (strcmp(color_type, "warn") == 0) {
        code = shell_config.warn_color;
    } else if (strcmp(color_type, "error") == 0) {
        code = shell_config.error_color;
    } else {
        LOG_WARN("Unknown color type passed: %s", color_type);
        return NULL;
    }

    for (size_t i = 0; i < sizeof(color_table)/sizeof(color_table[0]); i++) {
        if (color_table[i].code == code) {
            return color_table[i].name;
        }
    }

    static char buf[16];
    snprintf(buf, sizeof(buf), "%d", code);
    return buf;
}


bool set_info_color(const char *value) {
    return set_color("info", value);
}

bool set_warn_color(const char *value) {
    return set_color("warn", value);
}

bool set_error_color(const char *value) {
    return set_color("error", value);
}

const char *get_info_color() {
    return get_color("info");
}

const char *get_warn_color() {
    return get_color("warn");
}
const char *get_error_color() {
    return get_color("error");
}
