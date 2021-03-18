#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <microhttpd.h>

#include <def.h>
#include <log.h>
#include <http_connection.h>

typedef struct connection_context {
    struct MHD_PostProcessor *postprocessor; 
    char *answerstring;
    int connectiontype;
} connection_context;

static bool is_accordion_url(const char *url)
{
    return strstr(url, "/g/") != NULL;
}

static struct MHD_Response *create_entry_form_response()
{
    const char *form = "<html><head></head><body><h2>HTML Forms</h2><form action=\"/\" method=\"POST\"><label for=\"long_url\">Enter your URL:</label><br><input type=\"text\" id=\"long_url\" name=\"long_url\" value=\"https://www.duckduckgo.com\"><br><input type=\"submit\" value=\"Submit\"></form></body></html>";

    return MHD_create_response_from_buffer(
        strlen(form)
        , (void *) form
        , MHD_RESPMEM_PERSISTENT
    );
}

static struct MHD_Response *create_long_url_response(url_repo_t *repo, const char *url)
{
    char *long_url = fetch_long_url(repo, url);
    if (long_url == NULL) {
        error("unable to get original long url\n");
        return NULL;
    }
    debug("long url for %s is '%s'\n", url, long_url);

    struct MHD_Response *response = MHD_create_response_from_buffer(
        0
        , NULL
        , MHD_RESPMEM_PERSISTENT
    );
    MHD_add_response_header(response, MHD_HTTP_HEADER_LOCATION, long_url);

    free(long_url);

    return response;
}

static struct MHD_Response *create_accordion_url_response(url_repo_t *repo, const char *url)
{
    char *accordion_url = fetch_or_create_accordion_url(repo, url);
    if (accordion_url == NULL) {
        error("unable to get accordion url for %s\n", url);
        return NULL;
    }
    debug("accordion url for %s is %s\n", url, accordion_url);

    char page[256] = {0};
    snprintf(page, sizeof(page), "<html><body><p>AccordionURL: %s</p></body></html>", accordion_url);

    free(accordion_url);

    return MHD_create_response_from_buffer(
        strlen(page)
        , page
        , MHD_RESPMEM_PERSISTENT
    );
}

static enum MHD_Result get_response(struct MHD_Connection *connection, url_repo_t *repo, const char *url)
{
    debug("GET %s\n", url);
    
    struct MHD_Response *response = NULL;
    if (strcmp(url, "/") == 0) {
        response = create_entry_form_response();
    } else if (is_accordion_url(url)) {
        response = create_long_url_response(repo, url);
    } else {
        error("URL %s does not have a response handler for method GET\n", url);
        return MHD_NO;
    }

    // TODO: http found might not be right here anymore
    enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_FOUND, response);
    MHD_destroy_response(response);
    
    return ret;
}

enum MHD_Result iterate_headers(void *cls,
                        enum MHD_ValueKind kind,
                        const char *key,
                        const char *value)
{
    ACC_UNUSED(cls);
    ACC_UNUSED(kind);

    debug("Header: %s : Value: %s\n", key, value);

    return MHD_YES;
}

enum MHD_Result debug_print_headers(struct MHD_Connection *connection)
{
    return MHD_get_connection_values(connection, MHD_HEADER_KIND, iterate_headers, NULL);
}

// this is called each time a POST request is made on the connection
// for right now, we assume there is 1 request per connection
// but this is not a good assumption to have in general
static enum MHD_Result iterate_post(
    void *coninfo_cls
    , enum MHD_ValueKind kind
    , const char *key
    , const char *filename
    , const char *content_type
    , const char *transfer_encoding
    , const char *data
    , uint64_t off
    , size_t size
    )
{
    ACC_UNUSED(coninfo_cls);
    ACC_UNUSED(kind);
    ACC_UNUSED(filename);
    ACC_UNUSED(content_type);
    ACC_UNUSED(transfer_encoding);
    ACC_UNUSED(off);

    struct connection_context *cctx = coninfo_cls;

    if (strcmp(key, "long_url") == 0) {
        debug("long_url received in POST: %s\n", data);
        if (size > 0) {
            char *long_url = strdup(data);
            if (long_url == NULL) {
                error("unable to allocate memory for incoming POST data long_url\n");
                return MHD_NO;
            }
            cctx->answerstring = long_url;
            debug("answer string: %s\n", cctx->answerstring);
        } else {
            cctx->answerstring = NULL;
        }

        // done processing
        return MHD_NO;
    }

    return MHD_YES; // continue processing
}

static void request_completed(
    void *cls
    , struct MHD_Connection *connection
    , void **con_cls
    , enum MHD_RequestTerminationCode term_code
    )
{
    ACC_UNUSED(cls);
    ACC_UNUSED(connection);
    ACC_UNUSED(term_code);

    struct connection_context *cctx = *con_cls;
    if (cctx == NULL) {
        error("connection context not found on completed request\n");
        return;
    }

    if (cctx->connectiontype == POST) {
        MHD_destroy_post_processor(cctx->postprocessor);        
        if (cctx->answerstring != NULL) {
            free (cctx->answerstring);
        }
    }

    free(cctx);
    *con_cls = NULL;   
}

static enum MHD_Result post_response(
    struct MHD_Connection *connection
    , url_repo_t *repo
    , const char *url
    , connection_context *cctx
    , const char *upload_data
    , size_t *upload_data_size
    )
{
    debug("POST %s\n", url);

    // fetch body
    // parse out long url
    // create accordion url
    // return accordion url in a <p> response??

    if (strcmp(url, "/") != 0) {
        error("URL %s does not have a response handler for POST method\n", url);
        return MHD_NO;
    }

    debug("upload_data_size %d\n", *upload_data_size);
    if (*upload_data_size > 0) {
        MHD_post_process(cctx->postprocessor, upload_data, *upload_data_size);
        // this indicates that there is no more data to process
        // we're currently assuming there is only one request made per connection
        *upload_data_size = 0;

        return MHD_YES;
    }
    
    const char *ok = "<html><head></head><body><h1>OK %s</h1></body></html>";
    char buf[255] = {0};
    snprintf(buf, sizeof(buf), ok, cctx->answerstring);
    debug("POST answer: %s\n", buf);

    struct MHD_Response *response = MHD_create_response_from_buffer(strlen(buf), buf, MHD_RESPMEM_MUST_COPY);
    if (response == NULL) {
        error("unable to create HTTP POST response for %s\n", url);
        return MHD_NO;
    }
    MHD_add_response_header(response, MHD_HTTP_HEADER_CONTENT_TYPE, "text/html");

    enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    MHD_destroy_response(response);
    
    return ret;
}

enum MHD_Result setup_request_specific_context(struct MHD_Connection *connection, const char *method, void **shared_connection_data)
{
    connection_context *cctx = calloc(1, sizeof(*cctx));
    if (cctx == NULL) {
        error("unable to allocate memory for request specific shared data\n");
        return MHD_NO;
    }

    if (strcmp(method, "POST") == 0) {
        cctx->postprocessor = MHD_create_post_processor(connection, 4 * 1024, iterate_post, cctx);
        if (cctx->postprocessor == NULL) {
            error("unable to allocate memory for request postprocessor\n");
            free(cctx);
            return MHD_NO;
        }
        cctx->connectiontype = POST;
    } else {
        cctx->connectiontype = GET;
    }

    *shared_connection_data = cctx;

    return MHD_YES;
}

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

    url_repo_t *repo = (url_repo_t *)cls;
    if (repo == NULL) {
        fatal("please supply the repo into MHD\n");
    }

    if (debug_print_headers(connection) == MHD_NO) {
        error("unable to obtain connection values for url %s\n", url);
        return MHD_NO;
    }

    // con_cls will be NULL the first time this request is handled by the server
    // In that case, we set up a shared data context that will only be shared for this __specific__
    // request and not all requests.
    // This is useful for handling POST requests with large amounts of data in their body as these
    // may be split into multiple requests.
    if (*con_cls == NULL) {
        debug("setting up the connection specific data for initial connection request\n");
        return setup_request_specific_context(connection, method, con_cls);
    }

    debug("METHOD: %s, URL %s\n", method, url);

    if (strcmp(method, "GET") == 0) {
        return get_response(connection, repo, url);
    } else if (strcmp(method, "POST") == 0) {
        return post_response(connection, repo, url, (connection_context *)*con_cls, upload_data, upload_data_size);
    }

    // TODO: return not found response????
    return MHD_NO;
}

void start_http_daemon(url_repo_t *repo)
{
    struct MHD_Daemon *daemon = MHD_start_daemon(
        MHD_USE_INTERNAL_POLLING_THREAD
        , 8888 // TODO: add a way to customize this
        , NULL
        , NULL
        , answer_to_connection
        , repo
        , MHD_OPTION_NOTIFY_COMPLETED
        , request_completed
        , NULL
        , MHD_OPTION_END
    );
    
    if (daemon == NULL) {
        fatal("unable to start the HTTP daemon");
    } 

    // TODO: replace with SIGTERM signal handler to gracefully exit
    getchar();

    MHD_stop_daemon(daemon);
}