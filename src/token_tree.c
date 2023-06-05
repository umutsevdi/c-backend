#include "token_tree.h"
#include <stdio.h>

typedef struct __HTTPC_TREE_TOKEN__ {
    gchar** tokenv;
    int tokenc;
} TreeToken;

TreeToken* httpc_tokenize(gchar* path)
{
    TreeToken* t = g_new(TreeToken, 1);
    t->tokenv = g_strsplit(path, "/", -1);
    t->tokenc = g_strv_length(t->tokenv);
    return t;
}

int httpc_token_len(TreeToken* t)
{
    return t->tokenc;
}

void httpc_token_free(TreeToken* t)
{
    if (t == NULL)
        return;
    for (int i = 0; i < t->tokenc; i++)
        g_free(t->tokenv[i]);
    g_free(t);
}

typedef struct __HTTPC_TREE__ {
    gchar* token;
    gpointer data;
    GList* children; // List of child nodes
} Tree;

Tree* httpc_tree_new(gchar* token, gpointer data)
{
    Tree* node = g_new(Tree, 1);
    node->token = g_strdup(token);
    node->data = data;
    node->children = NULL;
    return node;
}

void httpc_tree_add(Tree* parent, Tree* child)
{
    parent->children = g_list_append(parent->children, child);
}

void httpc_tree_traverse(Tree* root)
{
    // Process the data of the current node
    // ...

    // Traverse the child nodes recursively
    GList* iter = root->children;
    while (iter != NULL) {
        Tree* child = iter->data;
        httpc_tree_traverse(child);
        iter = g_list_next(iter);
    }
}

void httpc_tree_free(Tree* root)
{
    if (root == NULL)
        return;

    g_list_free_full(root->children, (GDestroyNotify)httpc_tree_free);
    g_free(root);
}

gboolean httpc_tree_insert(Tree* parent, TreeToken* t, gpointer value, gboolean mkdir)
{
    if (parent == NULL || t == NULL || t->tokenv == NULL) {
        return FALSE;
    }
    // Traverse the tokens until the last one
    Tree* current = parent;
    for (int i = 0; i < t->tokenc - 1; i++) {
        gboolean found = FALSE;
        // Check if a child node with the token already exists
        GList* iter = current->children;
        while (iter != NULL) {
            Tree* child = iter->data;
            if (g_strcmp0(child->token, t->tokenv[i]) == 0) {
                current = child;
                found = TRUE;
                break;
            }
            iter = g_list_next(iter);
        }

        // If a child node doesn't exist and mkdir is enabled, create a new child node
        if (!found && mkdir) {
            Tree* new = httpc_tree_new(t->tokenv[i], NULL);
            current->children = g_list_append(current->children, new);
            current = new;
        } else if (!found && !mkdir) {
            // Return FALSE if a child node doesn't exist and mkdir is disabled
            return FALSE;
        }
    }

    // Create the last token's child node and set its value
    Tree* new = httpc_tree_new(t->tokenv[t->tokenc - 1], value);
    current->children = g_list_append(current->children, new);
    return TRUE;
}

gpointer __httpc_tree_get(Tree* tree, TreeToken* t, int __idx)
{
    /* Check if lookup index is out of bounds or if the current node's token
     * does not match */
    if (__idx >= t->tokenc || g_strcmp0(tree->token, t->tokenv[__idx]) != 0)
        return NULL;
    /* If this is the last token in the path, return the data associated with
     * the current node*/
    if (__idx == t->tokenc - 1)
        return tree->data;

    // Look for a child node with the next token in the path
    GList* iter = tree->children;
    while (iter != NULL) {
        Tree* child = iter->data;
        gpointer result = __httpc_tree_get(child, t, __idx + 1);
        if (result != NULL)
            return result; // Found a matching child node
        iter = g_list_next(iter);
    }

    return NULL; // No matching child node found
}

gpointer httpc_tree_get(Tree* tree, TreeToken* tree_token)
{
    return __httpc_tree_get(tree, tree_token, 0);
}

void httpc_tree_test()
{
    // Create nodes
    Tree* root = httpc_tree_new("root", "hello");
    Tree* node1 = httpc_tree_new("node_1", "node_value1");
    Tree* node2 = httpc_tree_new("node_2", "node_value2");
    Tree* node3 = httpc_tree_new("node_3", "node_value3");

    // Build the tree structure
    httpc_tree_add(root, node1);
    httpc_tree_add(root, node2);
    httpc_tree_add(node2, node3);
    TreeToken* t2 = httpc_tokenize("root/node5/1/2/3/4");
    TreeToken* t3 = httpc_tokenize("node5/1/2/3/4");
    gboolean done1 = httpc_tree_insert(root, t3, "hello 2", FALSE);
    gboolean done2 = httpc_tree_insert(root, t3, "hello 3", TRUE);
    printf("httpc_tree_insert#%s %d %d\n", (char*)httpc_tree_get(root, t2), done1, done2);
    TreeToken* t = httpc_tokenize("root/node_1");
    printf("%s", (char*)httpc_tree_get(root, t));
    TreeToken* t_null = httpc_tokenize("asdadsdsa");
    printf("%s", (char*)httpc_tree_get(root, t_null));
    httpc_token_free(t);
    httpc_token_free(t_null);
}
