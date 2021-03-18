#include <stdlib.h>

#include <http_connection.h>

#include <log.h>
#include <url_repo.h>

static int parse_int(const char *str)
{
    char *end = NULL;

    long port = strtol(str, &end, 10);
    if (*end != '\0') {
        fatal("%s is not a valid integer\n", str);
    }

    return (int) port;
}

int main (int argc, char **argv)
{
    url_repo_t repo;

    int port;
    if (argc == 1) {
        port = 8888;
    } else if (argc == 2) {
        port = parse_int(argv[1]);
    } else {
        fatal("%d arguments are not allowed", argc);
    }
    url_repo_init(&repo, port);
    
    start_http_daemon(&repo);
    
    url_repo_teardown(&repo);
}

