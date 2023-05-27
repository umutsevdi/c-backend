#include "util.h"
#include <stdio.h>
#define __ITEM_WITHOUT_VALUE "true"

GHashTable* util_parse_args(int argc, char* argv[])
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

GString* util_get_mimetype(const char* path)
{
    GString* basename = g_string_new(g_path_get_basename(path));
    /* ext is not a copy, so don't free*/
    gchar* ext = g_strrstr(basename->str, ".");
    gchar* mime = "text/html";
    GString* r;

    if (ext) {
        gboolean is_certain = FALSE;
        gchar* content_type = g_content_type_guess(basename->str, NULL, 0, &is_certain);
        if (content_type) {
            mime = g_content_type_get_mime_type(content_type);
            r = g_string_new(mime);
            g_free(mime);
            g_free(content_type);
            g_string_free(basename, TRUE);
            return r;
        }
    }
    g_string_free(basename, TRUE);
    return g_string_new(mime);
}
