#pragma once

#include <stdio.h>

#define error(str, ...) do {\
    fprintf(stderr, str,##__VA_ARGS__);\
} while (0)
