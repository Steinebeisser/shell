#include "builtin_util.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"

#include "core/shell_print.h"
#include "config/config.h"
#include "config/rc_parser.h"
#include "platform/strndup.h"

#include <string.h>

bool get_flags(int argc, const char **argv, int *flag_argc, char ***flag_argv, int *new_argc, char ***new_argv) {
    if (argc < 1) return false;

    int flag_count = 0;
    int non_flag_count = 1;

    // skipping cmd (argv[0])
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            non_flag_count += 1;
            continue;
        }

        if (argv[i][1] == '-') {
            flag_count += 1;
        } else {
            flag_count += strlen(argv[i]) - 1;
        }
    }

    char **flags = malloc(flag_count * sizeof(char *));
    char **normal_argv = malloc(non_flag_count * sizeof(char *));
    if (!flags || !normal_argv) goto cleanup;

    int non_flag_count_fill = 0;
    int flag_count_fill = 0;

    normal_argv[non_flag_count_fill++] = strdup(argv[0]);

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            normal_argv[non_flag_count_fill++] = strdup(argv[i]);
            continue;
        }

        if (argv[i][1] == '-') {
            flags[flag_count_fill++] = strndup(argv[i] + 2, strlen(argv[i]) - 2);
        } else {
            for (int j = 1; argv[i][j]; ++j) {
                char *flag = malloc(2);
                if (!flag) goto cleanup;
                flag[0] = argv[i][j];
                flag[1] = '\0';
                flags[flag_count_fill++] = flag;
            }
        }
    }

    if (flag_count_fill != flag_count) {
        shell_print(SHELL_ERROR, "get_flags: Unexpected flag count\n");
        LOG_ERROR("get_flags: Unexpected flag count\n");
        goto cleanup;
    }

    if (non_flag_count != non_flag_count_fill) {
        shell_print(SHELL_ERROR, "get_flags: Unexpected args count\n");
        LOG_ERROR("get_flags: Unexpected args count\n");
        goto cleanup;
    }

    *flag_argc = flag_count_fill;
    *flag_argv = flags;
    *new_argc = non_flag_count_fill;
    *new_argv = normal_argv;
    return true;

cleanup:
    if (flags) {
        for (int i = 0; i < flag_count_fill; ++i) free(flags[i]);
        free(flags);
    }
    if (new_argv) {
        for (int i = 0; i < non_flag_count_fill; ++i) free(normal_argv[i]);
        free(normal_argv);
    }
    return false;
}

char *merge_args(int start_at, int argc, const char **argv) {
    size_t needed = 1;
    int num_args = argc - start_at;
    if (num_args < 1) {
        LOG_DEBUG("Tried Merging args but num of args are less than 1, start_at %d, argc: %d", start_at, argc);
        return NULL;
    }
    LOG_DEBUG("Merging args starting at %d, for %d args, merge argc: %d", start_at, argc, num_args);
    for (int i = start_at; i < argc; ++i) {
        if (!argv[i]) continue;
        needed += strlen(argv[i]);
    }
    if (num_args > 1) {
        needed += num_args - 1; // spaces
    }

    char *merged_args = malloc(needed);
    LOG_DEBUG("Allocating %llu bytes for new merged args", needed);
    if (!merged_args) return NULL;

    size_t pos = 0;
    for (int i = start_at; i < argc; ++i) {
        if (!argv[i]) continue;

        size_t len = strlen(argv[i]);
        memcpy(merged_args + pos, argv[i], len);
        pos += len;

        if (i < argc - 1) merged_args[pos++] = ' ';
    }

    merged_args[pos] = '\0';
    return merged_args;
}

void handle_rc_command(const char *cmd, int argc, const char **argv) {
    if (strcmp(cmd, "set") != 0 && strcmp(cmd, "get") != 0 && strcmp(cmd, "unset") != 0) {
        shell_print(SHELL_ERROR, "Invalid Rc Command called: %s\n", cmd);
        return;
    }
    char **flag_argv = NULL;
    int flag_argc = 0;
    char **new_argv = NULL;
    int new_argc = 0;
    bool is_force = false;
    bool is_persistent = false;
    bool orignal_env_override_value = false;
    bool is_comment = false;

    if (!get_flags(argc, argv, &flag_argc, &flag_argv, &new_argc, &new_argv)) {
      shell_print(SHELL_ERROR, "Failed to get flags\n");
      LOG_ERROR("Failed to get flags");
      return;
    }

    for (int i = 0; i < flag_argc; ++i) {
      if (strcmp(flag_argv[i], "p") == 0 || strcmp(flag_argv[i], "persistence") == 0) {
        is_persistent = true;
      } else if (strcmp(flag_argv[i], "f") == 0 || strcmp(flag_argv[i], "force") == 0) {
        is_force = true;
      } else if (strcmp(flag_argv[i], "c") == 0 || strcmp(flag_argv[i], "comment_old") == 0) {
        is_comment = true;
      }
    }

    char *line = merge_args(0, new_argc, (const char**)new_argv);
    if (!line) {
      shell_print(SHELL_ERROR, "Failed to merge argv\n");
      LOG_ERROR("Failed to merge argv");
      goto set_cleanup2;
    }

    if (strcmp(argv[0], "get") == 0) {
      bool insta_print = true;
      get_rc_value(line, insta_print);
      goto set_cleanup2;
    }

    LOG_DEBUG("Parsing %s", line);
    if (is_force) {
      orignal_env_override_value = shell_config.allow_env_override;
      shell_config.allow_env_override = true;
    }

    if (!parse_rc_line(line)) {
      shell_print(SHELL_ERROR, "Failed to parse statement\n");
      LOG_ERROR("Failed to parse statement for line %s", line);
      goto set_cleanup1;
    }

    if (is_force) {
      shell_config.allow_env_override = orignal_env_override_value ;
    }

    bool is_set = (strcmp(argv[0], "set") == 0);;

    if (is_persistent) {
      if (is_set) {
        char *pline = strdup(temp_sprintf("%s : added via -p flag\n", (const char*)line));
        if (!pline) {
          LOG_ERROR("Failed to get line");
          goto set_cleanup1;
        }
        if (!append_rc_file((const char *)pline, is_force, is_comment)) {
          shell_print(SHELL_ERROR, "Failed to persist setting\n");
          LOG_ERROR("Failed to persist setting %s", line);
        }
        free(pline);
      } else {
        if (!remove_rc_value((const char*)line, is_force)) {
          shell_print(SHELL_ERROR, "Failed to persist setting\n");
          LOG_ERROR("Failed to persist setting %s", line);
        }
      }
    }

set_cleanup1:
    free(line);
set_cleanup2:
    for (int i = 0; i < flag_argc; ++i) free(flag_argv[i]);
    free(flag_argv);
    for (int i = 0; i < new_argc; ++i) free(new_argv[i]);
    free(new_argv);
}
