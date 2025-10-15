#include "builtins.h"

#define PGS_LOG_STRIP_PREFIX
#include "third_party/pgs_log.h"
#include "third_party/nob.h"


#define BUILTIN(name, usage, description, examples, flags, func) \
    { #name, usage, description, examples, flags, func },

Builtin builtin_table[] = {
    #include "builtins.def"
};

#undef BUILTIN
const size_t num_builtins = NOB_ARRAY_LEN(builtin_table);

int execute_builtin(int argc, const char **argv) {
    LOG_DEBUG("Testing if Builtin exists: %s, with %d additional argv", argv[0], argc - 1);

    for (size_t i = 0; i < NOB_ARRAY_LEN(builtin_table); ++i) {
        if (strcmp(builtin_table[i].name, argv[0]) == 0) {
            builtin_table[i].func(argc, argv);
            return 1;
        }
    }

    return 0;
}
