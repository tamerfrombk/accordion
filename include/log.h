#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACC_LOG_MAX_BUFFER_LENGTH (255)

#ifdef NDEBUG
    #define debug(str, ...)
#else
    #define debug(str, ...) do {\
        char debug_buf[ACC_LOG_MAX_BUFFER_LENGTH + 1] = {0};\
        strcpy(debug_buf, "DEBUG: ");\
        strncat(debug_buf, str, ACC_LOG_MAX_BUFFER_LENGTH - strlen("DEBUG: "));\
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
