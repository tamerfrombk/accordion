#pragma once

struct FILE;

typedef struct url_entry_t {
    const char *url;
    char *accordion_url;
    struct url_entry_t *next;
} url_entry_t;

typedef struct  {
    FILE *connection;
    url_entry_t *entries;
} url_repo_t;

void url_repo_init(url_repo_t *repo);
void url_repo_teardown(url_repo_t *repo);

char *fetch_or_create_accordion_url(url_repo_t *repo, const char *url);
char *fetch_accordion_url(url_repo_t *repo, const char *url);
char *create_accordion_url(url_repo_t *repo, const char *url);

char *fetch_long_url(url_repo_t *repo, const char *accordion_url);

char *fetch_hostname();