#pragma once

#include <hiredis/hiredis.h>
typedef struct  {
    redisContext *connection;           // the connection to the on-disk repository
    int port;                           // the port of this http server
} url_repo_t;

void url_repo_init(url_repo_t *repo, int port);
void url_repo_teardown(url_repo_t *repo);

char *fetch_or_create_accordion_url(url_repo_t *repo, const char *url);
char *fetch_accordion_url(url_repo_t *repo, const char *url);
char *create_accordion_url(url_repo_t *repo, const char *url);

char *fetch_long_url(url_repo_t *repo, const char *accordion_url);

char *fetch_hostname();
