#include <url.h>

#include <string.h>

size_t fetch_url(const char *long_url, size_t long_len, char *short_url, size_t short_len)
{
    strncpy(short_url, "this is a URL", short_len);
    
    return 0;
}