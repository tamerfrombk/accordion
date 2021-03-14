#pragma once

struct FILE;
typedef struct  {
    FILE *connection;
} url_repo_t;

void url_repo_init(url_repo_t *repo);
void url_repo_teardown(url_repo_t *repo);

char *fetch_or_create_accordion_url(url_repo_t *repo, const char *url);
char *fetch_accordion_url(url_repo_t *repo, const char *url);
char *create_accordion_url(url_repo_t *repo, const char *url);

char *fetch_hostname();