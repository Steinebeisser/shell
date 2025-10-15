#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "third_party/nob.h"
#define BUILD_DIR "build/"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    nob_log(NOB_INFO, "i work");
    if (!mkdir_if_not_exists(BUILD_DIR)) return 1;

    const char *defines[] = {
        // "DEBUG",
    };

    const char *includes[] = {
        ".",
        "src"
    };

    const char *src_files[] = {
        "src/core/main.c",
        "src/core/shell.c",
        "src/core/repl.c",
        "src/core/shell_print.c",
        "src/core/shell_input.c",
        "src/core/input_modes/normal_mode.c",
        "src/core/history.c",

        "src/commands/builtins.c",
        "src/commands/commands.c",
        "src/commands/util.c",
        "src/commands/builtins/builtin_exit.c",
        "src/commands/builtins/builtin_get.c",
        "src/commands/builtins/builtin_set.c",
        "src/commands/builtins/builtin_unset.c",
        "src/commands/builtins/builtin_util.c",
        "src/commands/builtins/builtin_source.c",
        "src/commands/builtins/builtin_help.c",

        "src/config/config.c",
        "src/config/rc_parser.c",
        "src/config/color_utils.c",
        "src/config/alias.c",
        "src/config/env.c",

        "src/platform/unix/externals.c",
        "src/platform/windows/externals.c",
        "src/platform/unix/terminal.c",
        "src/platform/windows/terminal.c",
        "src/platform/unix/input.c",
        "src/platform/windows/input.c",
    };

    Nob_Cmd cmd = {0};
    cmd_append(&cmd, "cc");
    cmd_append(&cmd, "-Wall");
    cmd_append(&cmd, "-Wextra");
    cmd_append(&cmd, "-g");
    cmd_append(&cmd, "-fsanitize=address");
    cmd_append(&cmd, "-fno-omit-frame-pointer");

    cmd_append(&cmd, "-o");
    cmd_append(&cmd, BUILD_DIR"shell");

    for (size_t i = 0; i < NOB_ARRAY_LEN(includes); ++i) {
        cmd_append(&cmd, temp_sprintf("-I%s", includes[i]));
    }

    for (size_t i = 0; i < NOB_ARRAY_LEN(defines); ++i) {
        cmd_append(&cmd, temp_sprintf("-D%s", defines[i]));
    }

    for (size_t i = 0; i < NOB_ARRAY_LEN(src_files); ++i) {
        cmd_append(&cmd, src_files[i]);
    }

    cmd_run(&cmd);

    return 0;
}

