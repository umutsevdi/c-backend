#include "router.h"
#include <stdio.h>
#define __ITEM_WITHOUT_VALUE "true"

enum MHD_Result request_handler(void* cls, struct MHD_Connection* connection,
    const char* url, const char* method,
    const char* version, const char* upload_data,
    size_t* upload_data_size, void** con_cls);

GHashTable* parse_args(int argc, char* argv[]);

void print_key_values(gpointer key, gpointer value, gpointer user_data)
{
    printf("Key: %s, Value: %s\n", (char*)key, (char*)value);
}

int main(int argc, char* argv[])
{
    GHashTable* table = parse_args(argc, argv);
    g_hash_table_foreach(table, print_key_values, NULL);

    struct MHD_Daemon* daemon;
    daemon = MHD_start_daemon(MHD_USE_THREAD_PER_CONNECTION, 8080,
        NULL, NULL, &request_handler, NULL, MHD_OPTION_END);

    if (daemon == NULL) {
        perror("Failed to start the server.\n");
    }

    printf("Server running on port 8080...\n");
    getchar(); // wait for a key to stop server
    MHD_stop_daemon(daemon);

    g_hash_table_destroy(table);
    return 0;
}

enum MHD_Result request_handler(void* cls, struct MHD_Connection* connection,
    const char* url, const char* method,
    const char* version, const char* upload_data,
    size_t* upload_data_size, void** con_cls)
{
    GString* g_response = g_string_new(
        "<html><body>"
        "<h1>Hello, World!</h1>"
        "<p> You requested: $ECHO</p>"
        "</body></html>");
    g_string_replace(g_response, "$ECHO", url, 1);

    struct MHD_Response* http_response;

    http_response = MHD_create_response_from_buffer(g_response->len,
        (void*)g_response->str,
        MHD_RESPMEM_PERSISTENT);
    int ret = MHD_queue_response(connection, MHD_HTTP_OK, http_response);
    MHD_destroy_response(http_response);
    g_string_free(g_response, 0);
    return ret;
}

GHashTable* parse_args(int argc, char* argv[])
{
    GHashTable* table = g_hash_table_new(g_str_hash, g_str_equal);
    char* current_key;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            if (current_key) {
                g_hash_table_insert(table, current_key, __ITEM_WITHOUT_VALUE);
            }
            current_key = argv[i];
        } else if (current_key) {
            g_hash_table_insert(table, current_key, argv[i]);
            current_key = NULL;
        } else {
            printf("Warning: Error while parsing {%s}.\n", argv[i]);
        }
    }
    return table;
}
