/******************************************************************************

 * File: include/router.h
 *
 * Author: Umut Sevdi
 * Created: 05/27/23
 * Description: Router tree header

*****************************************************************************/
/**
 *
 * Def
 * A -> B -> {}
 *
 * A/B/C : pass
 * A/B/D : pass
 * A/B/E/F : fail
 */

#ifndef __WEBSITE_ROUTER__
#define __WEBSITE_ROUTER__
#include <glib.h>
#include <microhttpd.h>
#include <stdio.h>
enum ROUTER_METHOD {
    ROUTER_METHOD_GET,
    ROUTER_METHOD_POST,
    ROUTER_METHOD_PUT,
    ROUTER_METHOD_DELETE,
    ROUTER_METHOD_PATCH,
    ROUTER_METHOD_HEAD
};

gchar* router_method_value(enum ROUTER_METHOD method);
enum ROUTER_METHOD router_value_of(gchar* string);

typedef struct {
    gchar* path;
    enum ROUTER_METHOD method;
    enum MHD_Result (*fn_ptr)(struct MHD_Connection* connection,
        const char* url, const char* upload_data,
        size_t* upload_data_size, int argc, const char** argv);
} Route;

Route* router_new(const char* path, enum ROUTER_METHOD method,
    enum MHD_Result (*fn_ptr)(struct MHD_Connection* connection,
        const char* url, const char* upload_data,
        size_t* upload_data_size, int argc, const char** argv));

void route_free(Route* r);
/**
 * router_bind - assigns a function to the given path in the node
 * @
 *
 *
 */
GNode* router_find(GTree* tree, const char* path);
void router_bind(GTree* tree, const char* path, enum ROUTER_METHOD method,
    enum MHD_Result (*fn_ptr)(struct MHD_Connection* connection,
        const char* url, const char* upload_data,
        size_t* upload_data_size, int argc, const char** argv));

void router_test();
#endif // !__WEBSITE_ROUTER__
