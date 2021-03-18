#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ACC_LOG_MAX_BUFFER_LENGTH (255)

#define ACC_LOG_IMPL(prefix, str, ...) do {\
        char debug_buf[ACC_LOG_MAX_BUFFER_LENGTH + 1] = {0};\
        strcpy(debug_buf, prefix);\
        strncat(debug_buf, str, ACC_LOG_MAX_BUFFER_LENGTH - strlen(prefix));\
        fprintf(stderr, debug_buf,##__VA_ARGS__);\
    } while (0)

#ifdef NDEBUG
    #define debug(str, ...)
#else
    #define debug(str, ...) ACC_LOG_IMPL("DEBUG: ", str,##__VA_ARGS__)
#endif

#define error(str, ...) ACC_LOG_IMPL("ERROR: ", str,##__VA_ARGS__)

#define fatal(str, ...) ACC_LOG_IMPL("FATAL: ", str,##__VA_ARGS__)
