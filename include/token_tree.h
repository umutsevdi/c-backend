#ifndef __HTTPC_TOKEN_TREE__
#define __HTTPC_TOKEN_TREE__
#include <glib.h>
#include <stdio.h>

typedef struct __HTTPC_TREE_TOKEN__ TreeToken;

/**
 * tokenize - Generate a TreeToken from given path
 */
TreeToken* httpc_tokenize(gchar* path);

int httpc_token_len(TreeToken* t);

void httpc_token_free(TreeToken* t);

typedef struct __HTTPC_TREE__ Tree;

Tree* httpc_tree_new(gchar* token, gpointer data);

void httpc_tree_traverse(Tree* root);

void httpc_tree_free(Tree* root);

/**
 * tree_insert - Inserts a value under the given tree token.
 * @parent - parent to insert
 * @t - TreeToken value, contains the path to the value,
 *      - TreeToken mustn't include the parent's token
 *      - It must list only the list of tokens under the parent
 * @value - value, can be NULL
 * @mkdir - when enabled, creates the non-existing TreeToken paths.
 *      - All newly created Tree's have NULL as value
 *
 * Return - whether insertion is successful or not
 */
gboolean httpc_tree_insert(Tree* parent, TreeToken* t,
    gpointer value, gboolean mkdir);
/**
 * tree_get - Traverses the tree and looks for the token, returns it's value
 * if exists
 * @tree - tree to traverse
 * @t - TreeToken value, contains the path to the value,
 *      - TreeToken must include the tree's token
 *
 * Return - value that tree node holds, NULL if tree node doesn't exist
 */
gpointer httpc_tree_get(Tree* tree, TreeToken* tree_token);

void httpc_tree_test();

#endif
