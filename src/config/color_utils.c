#include "color_utils.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "config.h"

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
        return false;
    }

    return true;

}
