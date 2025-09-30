#ifndef STEIN_SHELL_PLATFORM_GETLINE_H
#define STEIN_SHELL_PLATFORM_GETLINE_H

#include <stdlib.h>
#include <stdio.h>

#if defined(__unix__)
    #include <sys/types.h>
#elif defined(__WIN32)
    #if !defined(__MINGW64__)
    #include <stdint.h>
    typedef int64_t ssize_t; // mingw already has a typedef for ssize_t
    #endif

    static inline ssize_t getline(char **lineptr, size_t *n, FILE *stream) {
        if (lineptr == NULL || n == NULL || !stream) return -1;

        if (*lineptr == NULL || *n == 0) {
            *n = 128;
            *lineptr = malloc(*n);
            if (!*lineptr) return -1;
        }

        size_t pos = 0;
        int c;

        while ((c = fgetc(stream)) != EOF) {
            if (pos + 1 >= *n) {
                size_t new_size = *n * 2;
                char *new_ptr = realloc(*lineptr, new_size);
                if (!new_ptr) return 1;
                *lineptr = new_ptr;
                *n = new_size;
            }
            (*lineptr)[pos++] = (char)c;
            if ((char)c == '\n') break;
        }

        if (pos <= 1) return -1;
        (*lineptr)[pos] = '\0';
        if ((*lineptr)[pos] == EOF) return -1;

        return (ssize_t)pos;
    }
#endif

#endif // STEIN_SHELL_PLATFORM_GETLINE_H
