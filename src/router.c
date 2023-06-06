#include "router.h"
#include "token_tree.h"
#include <stdio.h>
#define METHOD_GET "GET"
#define METHOD_POST "POST"
#define METHOD_PUT "PUT"
#define METHOD_DELETE "DELETE"
#define METHOD_PATCH "PATCH"
#define METHOD_HEAD "HEAD"

HcTree* tree;

void hc_route_setup()
{
    tree = hc_tree_new("", NULL);
}

struct Route {
    /* Full path to the route */
    gchar* path;
    HcTreeToken* token;
    HcRouteFunction fn_ptr[6];
};

struct Route* hc_route_new(const char* path)
{
    struct Route* r = g_new(struct Route, 1);
    r->token = hc_tokenize((gchar*)path);
    r->path = hc_token_str(r->token);
    for (int i = 0; i < 6; i++)
        r->fn_ptr[i] = NULL;
    return r;
}

void hc_route_free(struct Route* r)
{
    hc_token_free(r->token);
    g_free(r->path);
    g_free(r);
}

gboolean hc_route_bind(const char* path, enum ROUTER_METHOD method, HcRouteFunction fn_ptr)
{
    HcTreeToken* token = hc_tokenize((gchar*)path);
    if (hc_token_len(token) == 0)
        return FALSE;
    struct Route* r = hc_route_new(path);
    r->fn_ptr[method] = fn_ptr;
    if (r == NULL || r->token == NULL || hc_token_len(r->token) == 0) {
        hc_route_free(r);
    }
    hc_tree_insert(tree, token, r, (void (*)(gpointer))hc_route_free, TRUE);
    return TRUE;
}

HcRouteFunction hc_route_match(const char* path, enum ROUTER_METHOD method)
{
    gchar* c = path[0] != '/' ? g_strjoin("/", path, NULL) : g_strdup(path);
    HcTreeToken* token = hc_tokenize(c);
    struct Route* r = hc_tree_get(tree, token);
    if (r == NULL) {
        return NULL;
    }

    g_free(c);
    hc_token_free(token);
    return r->fn_ptr[method];
}

gchar* hc_route_method_str(enum ROUTER_METHOD m)
{
    char* methods[] = {
        METHOD_GET,
        METHOD_POST,
        METHOD_PUT,
        METHOD_DELETE,
        METHOD_PATCH,
        METHOD_HEAD
    };
    return methods[m];
}

enum ROUTER_METHOD hc_router_value_of(gchar* string)
{
    if (g_str_equal(METHOD_GET, string))
        return ROUTER_METHOD_GET;
    else if (g_str_equal(METHOD_POST, string))
        return ROUTER_METHOD_POST;
    else if (g_str_equal(METHOD_PUT, string))
        return ROUTER_METHOD_PUT;
    else if (g_str_equal(METHOD_DELETE, string))
        return ROUTER_METHOD_DELETE;
    else if (g_str_equal(METHOD_PATCH, string))
        return ROUTER_METHOD_PATCH;
    else if (g_str_equal(METHOD_HEAD, string))
        return ROUTER_METHOD_HEAD;
    return ROUTER_METHOD_GET;
}

void hc_route_test()
{
    printf("START ROUTE TEST\n");
    struct Route* r = hc_route_new("/users/new/{}");
    struct Route* r2 = hc_route_new("/users/new/path");

    printf("hc_route_add\n");
    hc_route_bind("path/to/file", ROUTER_METHOD_GET, NULL);
    hc_route_bind("path/to/{}", ROUTER_METHOD_GET, NULL);
    hc_route_bind("path/to/{}", ROUTER_METHOD_GET, NULL);
    printf("hc_route_tree_get\n");
    HcTreeToken* token = hc_tokenize("/path/to/file");
    struct Route* r_tree = hc_tree_get(tree, token);
    if (r_tree != NULL) {
        printf("route get success: %s\n", r_tree->path);
    } else {
        printf("get failed\n");
    }
    hc_token_free(token);

    hc_route_free(r);
    hc_route_free(r2);
    printf("END HTTPC-TREE TEST\n");
}
