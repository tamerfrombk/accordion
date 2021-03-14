#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <microhttpd.h>

#include <def.h>
#include <log.h>
#include <url_repo.h>

#define PORT 8888

enum MHD_Result answer_to_connection(
    void *cls
    , struct MHD_Connection *connection
    , const char *url
    , const char *method
    , const char *version
    , const char *upload_data
    , size_t *upload_data_size
    , void **con_cls
    )
{
    ACC_UNUSED(version);
    ACC_UNUSED(upload_data);
    ACC_UNUSED(upload_data_size);
    ACC_UNUSED(con_cls);

    url_repo_t *repo = (url_repo_t *)cls;
    if (repo == NULL) {
        fatal("please supply the repo into MHD");
    }

    debug("METHOD: %s, URL %s\n", method, url);

    if (strcmp(method, "GET") != 0) {
        return MHD_NO;
    }

    if (strstr(url, "/g/") != NULL) {
        debug("url %s redirecting to self\n", url);
        return MHD_NO;
    }

    struct MHD_Response *response = MHD_create_response_from_buffer(
        0
        , NULL
        , MHD_RESPMEM_PERSISTENT
    );
    
    char *accordion_url = fetch_or_create_accordion_url(repo, url);
    if (accordion_url == NULL) {
        fatal("unable to get accordion url\n");
    }
    debug("accordion url for %s is '%s'\n", url, accordion_url);

    MHD_add_response_header(response, "Location", accordion_url);

    enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_FOUND, response);
    
    free(accordion_url);
    MHD_destroy_response(response);

    return ret;
}

static void start_daemon()
{
    url_repo_t repo;
    url_repo_init(&repo);

    struct MHD_Daemon *daemon = MHD_start_daemon(
        MHD_USE_INTERNAL_POLLING_THREAD
        , PORT
        , NULL
        , NULL
        , answer_to_connection
        , &repo
        , MHD_OPTION_END
    );
    
    if (daemon == NULL) {
        fatal("unable to start the HTTP daemon");
    } 

    getchar();

    url_repo_teardown(&repo);
    MHD_stop_daemon(daemon);
}

static void fetch_short_url(const char *url)
{
    url_repo_t repo;
    url_repo_init(&repo);

    char *accordion_url = fetch_or_create_accordion_url(&repo, url);
    if (accordion_url == NULL) {
        fatal("unable to get accordion url for %s\n", url);
    }

    puts(accordion_url);

    url_repo_teardown(&repo);
    free(accordion_url);
}

int main (int argc, char **argv)
{
    if (argc == 1) {
        start_daemon();
    } else if (argc == 2) {
        fetch_short_url(argv[1]);
    } else {
        fatal("%d arguments are not allowed", argc);
    }
}

