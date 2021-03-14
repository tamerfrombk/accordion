#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef NDEBUG
    #define debug(str, ...)
#else
    #define debug(str, ...) do {\
        char debug_buf[256] = {0};\
        strcpy(debug_buf, "DEBUG: ");\
        strncat(debug_buf, str, sizeof(debug_buf) - 7 - 1);\
        fprintf(stderr, debug_buf,##__VA_ARGS__);\
    } while (0)
#endif

#define error(str, ...) do {\
    fprintf(stderr, str,##__VA_ARGS__);\
} while (0)

#define fatal(str, ...) do {\
    fprintf(stderr, str,##__VA_ARGS__);\
    exit(EXIT_FAILURE); \
} while (0)
