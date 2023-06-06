#include "router.h"
#include "token_tree.h"
#include "util.h"
#include <microhttpd.h>
#include <stdio.h>
#include <stdlib.h>

#define PAGE "<html><head><title>libmicrohttpd demo</title>" \
             "</head><body>libmicrohttpd demo!!</body></html>"

typedef struct {
    int port;
    char* name;
} Config;

Config map_config(GHashTable* table)
{
    Config c;
    gpointer port_ptr = g_hash_table_lookup(table, "--port");
    if (port_ptr == NULL)
        port_ptr = g_hash_table_lookup(table, "-p");
    if (port_ptr != NULL)
        c.port = atoi((char*)port_ptr);
    else
        c.port = 8080;

    gpointer name_ptr = g_hash_table_lookup(table, "--name");
    if (name_ptr == NULL)
        name_ptr = g_hash_table_lookup(table, "-n");

    if (name_ptr != NULL)
        c.name = g_strdup(name_ptr);
    else
        c.name = g_strdup("httpc");
    return c;
}

enum MHD_Result request_handler(void* cls, struct MHD_Connection* connection,
    const char* url, const char* method,
    const char* version, const char* upload_data,
    size_t* upload_data_size, void** con_cls);

void print_key_values(gpointer key, gpointer value, gpointer _)
{
    printf("[%-5s:%-5s]\n", (char*)key, (char*)value);
}

int main(int argc, char* argv[])
{
    hc_tree_test();
    hc_route_setup();
    hc_route_test();
    GHashTable* table = util_parse_args(argc, argv);
    g_hash_table_foreach(table, print_key_values, NULL);
    Config c = map_config(table);
    if (c.name != NULL)
        g_set_application_name(c.name);
    struct MHD_Daemon* daemon;
    daemon = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY, c.port,
        NULL, NULL, &request_handler, PAGE, MHD_OPTION_END);

    if (daemon == NULL) {
        perror("Failed to start the server.\n");
        exit(EXIT_FAILURE);
    }

    printf("Server %s started on port %d\n", c.name, c.port);
    getchar(); // wait for a key to stop server
    MHD_stop_daemon(daemon);
    g_hash_table_destroy(table);
    free(c.name);
    return 0;
}

enum MHD_Result request_handler(void* cls, struct MHD_Connection* connection,
    const char* url, const char* method,
    const char* version, const char* upload_data,
    size_t* upload_data_size, void** con_cls)
{
    printf("REQUEST{%s, %s, %s, %s, %s}\n", (char*)cls, url, method, version, (char*)con_cls[0]);
    printf("Data: %.*s\n", (int)*upload_data_size, upload_data);
    if (*upload_data_size > 0) {
        printf("BODY: %.*s\n", (int)(*upload_data_size), upload_data);
    }

    const char* type = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_TYPE);
    const char* len = MHD_lookup_connection_value(connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_LENGTH);
    printf("CONTENT-TYPE: %s %s\n", type, len);

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
