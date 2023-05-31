#include "router.h"
#include <stdio.h>
#define METHOD_GET "GET"
#define METHOD_POST "POST"
#define METHOD_PUT "PUT"
#define METHOD_DELETE "DELETE"
#define METHOD_PATCH "PATCH"
#define METHOD_HEAD "HEAD"

enum ROUTER_METHOD router_value_of(gchar* string)
{
    if (g_str_equal(METHOD_GET, string))
        return ROUTER_METHOD_GET;
    if (g_str_equal(METHOD_POST, string))
        return ROUTER_METHOD_POST;
    if (g_str_equal(METHOD_PUT, string))
        return ROUTER_METHOD_PUT;
    if (g_str_equal(METHOD_DELETE, string))
        return ROUTER_METHOD_DELETE;
    if (g_str_equal(METHOD_PATCH, string))
        return ROUTER_METHOD_PATCH;
    if (g_str_equal(METHOD_HEAD, string))
        return ROUTER_METHOD_HEAD;
    return ROUTER_METHOD_GET;
}

gchar* router_method_value(enum ROUTER_METHOD method)
{
    switch (method) {
    case ROUTER_METHOD_GET:
        return METHOD_GET;
    case ROUTER_METHOD_POST:
        return METHOD_POST;
    case ROUTER_METHOD_PUT:
        return METHOD_PUT;
    case ROUTER_METHOD_DELETE:
        return METHOD_DELETE;
    case ROUTER_METHOD_PATCH:
        return METHOD_PATCH;
    case ROUTER_METHOD_HEAD:
        return METHOD_HEAD;
    }
}

static gint path_cmp(gconstpointer a, gconstpointer b, gpointer user_data)
{
    return g_strcmp0(a, b);
}

Route* route_new(const char* path, enum ROUTER_METHOD method,
    enum MHD_Result (*fn_ptr)(struct MHD_Connection* connection,
        const char* url, const char* upload_data,
        size_t* upload_data_size, int argc, const char** argv))
{
    Route* route = g_new(Route, 1);
    route->path = g_strdup(path);
    route->method = method;
    route->fn_ptr = fn_ptr;
    return route;
}

void route_free(Route* r)
{
    free(r->path);
    free(r);
}

void router_bind(GTree* tree, const char* path, enum ROUTER_METHOD method,
    enum MHD_Result (*fn_ptr)(struct MHD_Connection* connection,
        const char* url, const char* upload_data,
        size_t* upload_data_size, int argc, const char** argv))
{

    gchar** components = g_strsplit(path, "/", -1); // Split the path into components

    GTreeNode* current_node = (GTreeNode*)g_tree_get_root(tree);

    for (gchar** iter = components; *iter != NULL; ++iter) {
        const gchar* component = *iter;
        GTreeNode* child = g_tree_lookup(current_node, component);

        if (child == NULL) {
            // Create a new child node if it doesn't exist
            child = g_tree_node_new(component);
            g_tree_node_insert(current_node, child);
        }

        current_node = child;
    }

    // Create a new Route instance and assign it to the leaf node
    Route* route = route_new(path, method, fn_ptr);

    // Assign method and fn_ptr fields here

    g_tree_node_set_value(current_node, route);

    // Free memory
    g_strfreev(components);
}
