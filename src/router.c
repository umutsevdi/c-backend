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
    /* Name of the file */
    gchar* file;
    /* Full path to the route */
    gchar** path;
    int depth;
    RouteFunction fn_ptr[6];
};

struct Route* route_new(const char* path)
{
    struct Route* r = g_new(struct Route, 1);
    r->path = g_strsplit(path, "/", -1);
    r->depth = g_strv_length(r->path);
    r->file = r->path[r->depth - 1];
    for (int i = 0; i < 6; i++)
        r->fn_ptr[i] = NULL;
    return r;
}

void route_free(struct Route* r)
{
    for (int i = 0; i < r->depth; i++) {
        g_free(r->path[i]);
    }
    g_free(r);
}

gchar* strv_dup_until(gchar** array, int until)
{
    gchar** new_array = g_memdup2(array, sizeof(gchar*) * (until + 1));
    gchar* path_str = g_strjoinv("/", new_array);
    printf("strv_dup_until#%d:%s\n", until + 1, path_str);
    g_free(new_array);
    return path_str;
}

void tree_traverse(struct Route* r)
{
    GTree* current = tree;
    for (int i = 0; i < r->depth; i++) {
        printf("FOR %d\n", i);
        if (strnlen(r->path[i], INITIAL_STR_LOOKUP_SIZE) == 0) {
            continue;
        }
        gpointer child = g_tree_lookup(current, r->path[i]);
        printf("AT %s\n", r->path[i]);

        if (child == NULL) {
            printf("node not found - inserting\n");
            gchar* path_until = strv_dup_until(r->path, i);
            printf("path_until:%s\n", path_until);
            struct Route* current_r = route_new(path_until);
            g_tree_insert(child, r->path[i], current_r);
            g_free(path_until);
            printf("node inserted %s\n", current_r->path[i]);
            child = g_tree_lookup(current, r->path[i]);
            if (child == NULL) {
                printf("WHAT THE FUCK\n");
            }
        } else {
            printf("node found\n");
        }
        current = (GTree*)child;
    }
}

gboolean route_add(const char* path, enum ROUTER_METHOD method, RouteFunction fn_ptr)
{

    TreeToken* token = httpc_tokenize((gchar*)path);
    if (httpc_token_len(token) == 0)
        return FALSE;

    struct Route* r = route_new(path);
    if (r == NULL || r->depth == 0 || r->path == NULL) {
        route_free(r);
    }
    tree_traverse(r);
    r->fn_ptr[method] = fn_ptr;
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

gint key_cmp(gconstpointer a, gconstpointer b, gpointer user_data)
{
    printf("key cmp\t%s,%s\n", (char*)a, (char*)b);
    return g_strcmp0(a, b);
}
gint route_cmp(gconstpointer a, gconstpointer b, gpointer user_data)
{
    printf("router cmp\n");
    const struct Route* r1 = a;
    const struct Route* r2 = b;

    int min_d = r1->depth > r2->depth ? r2->depth : r1->depth;
    if (r1->depth != r2->depth) {
        return r1->depth - r2->depth;
    }
    for (int i = 0; i < min_d; i++) {
        int r = g_strcmp0(r1->path[i], r2->path[2]);
        if (r != 0)
            return r;
    }
    return 0;
}

void router_test()
{
    struct Route* r = route_new("/users/new/{}");
    struct Route* r2 = route_new("/users/new/path");

    route_add("path/to/file", ROUTER_METHOD_GET, NULL);
    route_add("path/to/{}", ROUTER_METHOD_GET, NULL);
    route_free(r);
    route_free(r2);
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
