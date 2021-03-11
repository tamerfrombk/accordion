#pragma once

#include <stddef.h>

size_t fetch_url(const char *long_url, size_t long_len, char *short_url, size_t short_len);