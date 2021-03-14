#include <stdio.h>

#include <url_repo.h>
#include <log.h>

void url_repo_init(url_repo_t *repo)
{
    repo->connection = fopen("accordion.csv", "a+");
    if (repo->connection == NULL) {
        fatal("unable to initialize repo\n");        
    }
}

void url_repo_teardown(url_repo_t *repo)
{
    fclose(repo->connection);
}

char *fetch_or_create_accordion_url(url_repo_t *repo, const char *url)
{
    char *accordion_url = fetch_accordion_url(repo, url);

    if (accordion_url == NULL) {
        debug("unable to fetch accordion url for %s\n", url);
        return create_accordion_url(repo, url);
    }

    return accordion_url;
}

char *fetch_accordion_url(url_repo_t *repo, const char *url)
{
    debug("fetching accordion url for %s\n", url);

    return NULL;
}

char *create_accordion_url(url_repo_t *repo, const char *url)
{
    debug("creating accordion url for %s\n", url);

    return strdup("https://www.google.com");
}