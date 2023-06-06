#include "router.h"
#include "token_tree.h"
#include <stdio.h>
#define METHOD_GET "GET"
#define METHOD_POST "POST"
#define METHOD_PUT "PUT"
#define METHOD_DELETE "DELETE"
#define METHOD_PATCH "PATCH"
#define METHOD_HEAD "HEAD"

#define ROUTE_WILDCARD_INT "{int}"
#define ROUTE_WILDCARD_FLOAT "{float}"
#define ROUTE_WILDCARD_STR "{str}"
#define INITIAL_STR_LOOKUP_SIZE 1024

/* standard compare function for Routes */
gint route_cmp(gconstpointer a, gconstpointer b, gpointer user_data);
gint key_cmp(gconstpointer a, gconstpointer b, gpointer user_data);
/* Returns a non-zero value if the value is one of the wildcards.
 * int   = 1, float = 2, str   = 3 */
int is_wildcard(gconstpointer a);

Tree* tree;

void router_setup()
{
    tree = httpc_tree_new("", NULL);
}

struct Route {
    /* Full path to the route */
    gchar* path;
    TreeToken* token;
    RouteFunction fn_ptr[6];
};

struct Route* route_new(const char* path)
{
    struct Route* r = g_new(struct Route, 1);
    r->token = httpc_tokenize((gchar*)path);
    r->path = httpc_token_str(r->token);
    for (int i = 0; i < 6; i++)
        r->fn_ptr[i] = NULL;
    return r;
}

void route_free(struct Route* r)
{
    httpc_token_free(r->token);
    g_free(r->path);
    g_free(r);
}

gboolean route_add(const char* path, enum ROUTER_METHOD method, RouteFunction fn_ptr)
{

    TreeToken* token = httpc_tokenize((gchar*)path);
    if (httpc_token_len(token) == 0)
        return FALSE;
    struct Route* r = route_new(path);
    r->fn_ptr[method] = fn_ptr;
    if (r == NULL || r->token == NULL || httpc_token_len(r->token) == 0) {
        route_free(r);
    }
    httpc_tree_insert(tree, token, r, (void (*)(gpointer))route_free, TRUE);
    return TRUE;
}

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
    char* method_strv[] = {
        METHOD_GET,
        METHOD_POST,
        METHOD_PUT,
        METHOD_DELETE,
        METHOD_PATCH,
        METHOD_HEAD
    };
    return method_strv[method];
}

void router_test()
{
    printf("START ROUTE TEST\n");
    struct Route* r = route_new("/users/new/{}");
    struct Route* r2 = route_new("/users/new/path");

    printf("route_add\n");
    route_add("path/to/file", ROUTER_METHOD_GET, NULL);
    route_add("path/to/{}", ROUTER_METHOD_GET, NULL);
    route_add("path/to/{}", ROUTER_METHOD_GET, NULL);
    printf("route_tree_get\n");
    TreeToken* token = httpc_tokenize("/path/to/file");
    struct Route* r_tree = httpc_tree_get(tree, token);
    if (r_tree != NULL) {
        printf("route get success: %s\n", r_tree->path);
    } else {
        printf("get failed\n");
    }
    httpc_token_free(token);

    route_free(r);
    route_free(r2);
    printf("END HTTPC-TREE TEST\n");
}

int is_wildcard(gconstpointer a)
{
    if (g_strcmp0(a, ROUTE_WILDCARD_INT) == 0)
        return 1;
    else if (g_strcmp0(a, ROUTE_WILDCARD_FLOAT) == 0)
        return 2;
    else if (g_strcmp0(a, ROUTE_WILDCARD_STR) == 0)
        return 3;
    return 0;
}
