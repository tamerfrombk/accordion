#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <microhttpd.h>

#include <def.h>

#define PORT 8888

enum MHD_Result answer_to_connection (void *cls, struct MHD_Connection *connection,
                          const char *url,
                          const char *method, const char *version,
                          const char *upload_data,
                          size_t *upload_data_size, void **con_cls)
{
    ACC_UNUSED(cls);
    ACC_UNUSED(url);
    ACC_UNUSED(method);
    ACC_UNUSED(version);
    ACC_UNUSED(upload_data);
    ACC_UNUSED(upload_data_size);
    ACC_UNUSED(con_cls);

    const char *page  = "<html><body>Hello, browser!</body></html>";

    struct MHD_Response *response = MHD_create_response_from_buffer(strlen (page),
                                            (void*) page, MHD_RESPMEM_PERSISTENT);

    enum MHD_Result ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
    
    MHD_destroy_response(response);

    return ret;
}

int main ()
{
    struct MHD_Daemon *daemon = MHD_start_daemon(
        MHD_USE_INTERNAL_POLLING_THREAD
        , PORT
        , NULL
        , NULL
        , answer_to_connection
        , NULL
        , MHD_OPTION_END
    );
    
    if (daemon == NULL) {
        return EXIT_FAILURE;
    } 

    getchar();

    MHD_stop_daemon(daemon);
}

