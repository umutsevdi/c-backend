#include "router.h"
#include "util.h"
#include <stdio.h>

enum MHD_Result request_handler(void* cls, struct MHD_Connection* connection,
    const char* url, const char* method,
    const char* version, const char* upload_data,
    size_t* upload_data_size, void** con_cls);

void print_key_values(gpointer key, gpointer value, gpointer user_data)
{
    printf("Key: %s, Value: %s\n", (char*)key, (char*)value);
}

int main(int argc, char* argv[])
{
    g_set_application_name("My Web Server");
    GHashTable* table = util_parse_args(argc, argv);
    g_hash_table_foreach(table, print_key_values, NULL);
    g_hash_table_destroy(table);
    struct MHD_Daemon* daemon;
    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8080,
        NULL, NULL, &request_handler, NULL, MHD_OPTION_END);

    if (daemon == NULL) {
        perror("Failed to start the server.\n");
    }

    printf("Server running on port 8080...\n");
    getchar(); // wait for a key to stop server
    MHD_stop_daemon(daemon);
    return 0;
}

enum MHD_Result request_handler(void* cls, struct MHD_Connection* connection,
    const char* url, const char* method,
    const char* version, const char* upload_data,
    size_t* upload_data_size, void** con_cls)
{
    GString* g_response = g_string_new("<!DOCTYPE html><html lang=\"en\">"
                                       "<head><meta charset=\"UTF-8\" /></head>"
                                       "<body>"
                                       "<h1>Hello, World!</h1>"
                                       "<p> You requested: $ECHO</p>"
                                       "</body></html>");

    g_string_replace(g_response, "$ECHO", url, 1);

    struct MHD_Response* http_response = MHD_create_response_from_buffer(
        g_response->len,
        (void*)g_response->str,
        MHD_RESPMEM_PERSISTENT);

    GString* mime = util_get_mimetype(url);

    MHD_add_response_header(http_response, "Content-Type", mime->str);
    gchar* content_length = g_strdup_printf("%zu", g_response->len);
    MHD_add_response_header(http_response, "Content-Length", content_length);

    int ret = MHD_queue_response(connection, MHD_HTTP_OK, http_response);
    MHD_destroy_response(http_response);
    g_string_free(mime, TRUE);
    g_free(content_length);
    /*whether created on stack or not*/
    g_string_free(g_response, FALSE);
    return ret;
}
