/******************************************************************************

 * File: include/util.h
 *
 * Author: @umutsevdi
 * Created: 05/27/23
 * Description: Various functions

*****************************************************************************/
#ifndef __WEBSITE_UTIL__
#define __WEBSITE_UTIL__
#include <gio/gio.h>
#include <glib.h>
#include <stdio.h>

#define MAX_FILE_PATH_NAME 2048
/**
 * Parses arguments of the program and returns a hash table that contains them.
 *
 * - Arguments are stored as key value pairs,
 * - Arguments with no values are placed with "true" values
 *
 */
GHashTable* util_parse_args(int argc, char* argv[]);

/**
 * Returns the mime type of the file at given path
 * - Defaults to text/html if the mime-type is not recognized
 */
GString* util_get_mimetype(const char* path);

#endif
