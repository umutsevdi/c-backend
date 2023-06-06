/******************************************************************************

 * File: include/router.h
 *
 * Author: @umutsevdi
 * Created: 06/06/23
 * Description: Token tree header

*****************************************************************************/
#ifndef __HTTPC_TOKEN_TREE__
#define __HTTPC_TOKEN_TREE__

#include <glib.h>
#include <stdio.h>

typedef struct __HTTPC_TREE_TOKEN__ HcTreeToken;

/**
 * tokenize - Generate a TreeToken from given path
 */
HcTreeToken* hc_tokenize(gchar* path);

guint hc_token_len(HcTreeToken* t);

gchar* hc_token_str(HcTreeToken* t);

void hc_token_free(HcTreeToken* t);

typedef struct __HTTPC_TREE__ HcTree;

HcTree* hc_tree_new(gchar* token, gpointer data);

void hc_tree_traverse(HcTree* root);

void hc_tree_free(HcTree* root);

/**
 * Inserts a value under the given tree token. If a value already exists under
 * the path, overwrites it
 *
 * @parent  to insert
 * @t       token that contains the path to the value,
 *          - TreeToken mustn't include the parent's token
 *          - It must list only the list of tokens under the parent
 * @value   value to assign, can be NULL
 * @free_fn function to free the older value if overwrite is needed
 *          - free_fn: Function to free the value tree holds
 * @mkdir   when enabled, creates the non-existing TreeToken paths.
 *          - All newly created Tree's have NULL as value
 *
 * @return  whether insertion is successful or not
 */
gboolean hc_tree_insert(HcTree* parent, HcTreeToken* t, gpointer value,
    void (*free_fn)(gpointer mem), gboolean mkdir);

/**
 * Traverses the tree and looks for the token, returns it's value, controls the wildcard case
 * if exists
 * @tree   to traverse
 * @t      TreeToken value, contains the path to the value,
 *          - TreeToken must include the tree's token
 *
 * @return value that tree node holds, NULL if tree node doesn't exist
 */
gpointer hc_tree_get(HcTree* tree, HcTreeToken* tree_token);

void hc_tree_test();

#endif
