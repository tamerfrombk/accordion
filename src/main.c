#include <http_connection.h>

#include <log.h>
#include <url_repo.h>

static void fetch_short_url(url_repo_t *repo, const char *url)
{
    char *accordion_url = fetch_or_create_accordion_url(repo, url);
    if (accordion_url == NULL) {
        fatal("unable to get accordion url for %s\n", url);
    }

    puts(accordion_url);

    free(accordion_url);
}

int main (int argc, char **argv)
{
    url_repo_t repo;
    url_repo_init(&repo);

    if (argc == 1) {
        start_http_daemon(&repo);
    } else if (argc == 2) {
        fetch_short_url(&repo, argv[1]);
    } else {
        fatal("%d arguments are not allowed", argc);
    }
    
    url_repo_teardown(&repo);
}

