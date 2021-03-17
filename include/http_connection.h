#pragma once

#include <url_repo.h>

typedef enum http_method {
    GET = 0,
    POST
} http_method;

void start_http_daemon(url_repo_t *repo);
