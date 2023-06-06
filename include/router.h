/******************************************************************************

 * File: include/router.h
 *
 * Author: @umutsevdi
 * Created: 05/27/23
 * Description: Router tree header

*****************************************************************************/

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
typedef enum MHD_Result (*HcRouteFunction)(struct MHD_Connection* connection,
    const char* url, const char* upload_data,
    size_t* upload_data_size, int argc, const char** argv);

/** Allocates a router tree */
void hc_route_setup();

/** Returns the string representation of the enum */
gchar* hc_route_method_str(enum ROUTER_METHOD m);

/** Converts given string to a ROUTER_METHOD */
enum ROUTER_METHOD hc_router_value_of(gchar* string);

struct Route* hc_route_new(const char* path);
void hc_route_free(struct Route* r);

/**
 * Adds a function to respond on given path with dedicated
 * ROUTER_METHOD.
 *
 * @path path to the point, may include following wildcard characters:
 *     * {int}    any integer value
 *     * {float}  any float value
 *     * {str}    any string value
 * @method which Http ROUTER_METHOD to assign
 * @fn_ptr function pointer to assign
 *
 * This function will be assigned to the given HTTP method type for the
 * path and will be executed whenever a request is made to the given path.
 *
 * If a function for given path and HTTP method is defined, it will be replaced.
 *
 * @return whether assignment is completed or not
 */
gboolean hc_route_bind(const char* path, enum ROUTER_METHOD method,
    HcRouteFunction fn_ptr);
/**
 * Attempts to find an assigned function in given path
 * @path   path to function
 *      - If path is not found, wild cards are controlled
 * @method method type
 *
 * @return Route function
 *      - Returns NULL if the function is not found
 */
HcRouteFunction* hc_route_match(const char* path, enum ROUTER_METHOD method);

void hc_route_test();

#endif // !__HTTPC_ROUTER__
