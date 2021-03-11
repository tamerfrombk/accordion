#pragma once

#include <stdio.h>
#include <stdlib.h>

#define error(str, ...) do {\
    fprintf(stderr, str,##__VA_ARGS__);\
} while (0)

#define fatal(str, ...) do {\
    fprintf(stderr, str,##__VA_ARGS__);\
    exit(EXIT_FAILURE); \
} while (0)
