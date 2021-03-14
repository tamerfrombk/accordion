#include <stdio.h>

#include <url_repo.h>
#include <log.h>

static char ALPHABET[] = "abcdefghijklmnopqrstuvwxyz";

static char generate_random_char() {
    int i = rand() % 26;

    return ALPHABET[i];
}

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

    char *accordion_url = calloc(256, sizeof(*accordion_url));
    if (accordion_url == NULL) {
        return NULL;
    }

    char suffix[10] = {0};
    for (int i = 0; i < sizeof(suffix) - 1; ++i) {
        suffix[i] = generate_random_char();
    }

    const char *hostname = fetch_hostname();
    if (hostname == NULL) {
        error("unable to resolve hostname\n");
        return NULL;
    }

    int port = 8888;
    snprintf(accordion_url, 256, "http://%s:%d/g/%s", hostname, port, suffix);

    free(hostname);

    return accordion_url;
}

char *fetch_hostname()
{
    FILE *f = fopen("/etc/hostname", "r");
    if (f == NULL) {
        debug("unable to open /etc/hostname\n");
        return NULL;
    }

    char *hostname = calloc(32, sizeof(*hostname));
    if (hostname == NULL) {
        debug("unable to allocate hostname memory\n");
        fclose(f);
        return NULL;
    }

    size_t n = fread(hostname, sizeof(*hostname), 32 - 1, f);
    fclose(f);
    
    hostname[n] = '\0';
    hostname[n - 1] = '\0';

    return hostname;
}