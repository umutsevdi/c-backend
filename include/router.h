/******************************************************************************

 * File: include/router.h
 *
 * Author: @umutsevdi
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

#ifndef __HTTPC_ROUTER__
#define __HTTPC_ROUTER__
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

/* Allocates a router tree */
void router_setup();
/* Returns the string representation of the ROUTER_METHOD */
gchar* router_method_value(enum ROUTER_METHOD method);
/* Converts given string to a ROUTER_METHOD */
enum ROUTER_METHOD router_value_of(gchar* string);

struct Route;
/**
 * RouteFunction- A callback function that responds to assigned
 * path and method
 * ---
 *
 * enum MHD_Result (*fn_ptr)(connection, URL, upload_data,
        upload_data_size, args);
 *
 * ---
 *
 * @connection - micro-http connection pointer
 * @URL - the URL client requested
 * @upload_data - any data  transferred along with the request
 * @upload_data_size - data size
 * @args - key value pair of given arguments
 *
 */
typedef enum MHD_Result (*RouteFunction)(struct MHD_Connection* connection,
    const char* url, const char* upload_data,
    size_t* upload_data_size, int argc, const char** argv);

/**
 * route_add - adds a function to respond on given path with dedicated
 * ROUTER_METHOD.
 *
 * @path - path to the point, may include following wildcard characters:
 *     - {int}   - any integer value
 *     - {float} - any float value
 *     - {str}   - any string value
 * @method - which Http ROUTER_METHOD to assign
 * @fn_ptr - function pointer to assign
 * This function will be assigned to the given HTTP method type for the
 * path and will be executed whenever a request is made to the given path.
 *
 * If a function for given path and HTTP method is defined, it will be replaced.
 * Return - whether assignment is completed or not
 *
 * ---
 *
 * enum MHD_Result (*fn_ptr)(connection, URL, upload_data,
        upload_data_size, args);
 *
 * @connection - micro-http connection pointer
 * @URL - the URL client requested
 * @upload_data - any data  transferred along with the request
 * @upload_data_size - data size
 * @args - key value pair of given arguments
 *
 *
 */
gboolean route_add(const char* path, enum ROUTER_METHOD method,
    RouteFunction fn_ptr);
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

#endif // !__HTTPC_ROUTER__
