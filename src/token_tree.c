#include "token_tree.h"
#include <stdio.h>
#include <string.h>

/******************************************************************************
                             TREE-TOKEN
*****************************************************************************/

typedef struct __HTTPC_TREE_TOKEN__ {
    gchar** tokenv;
    guint tokenc;
} HcTreeToken;

HcTreeToken* hc_tokenize(gchar* path)
{
    HcTreeToken* t = g_new(HcTreeToken, 1);
    t->tokenv = g_strsplit(path, "/", -1);
    t->tokenc = g_strv_length(t->tokenv);
    return t;
}

gchar* hc_token_str(HcTreeToken* t)
{
    return g_strjoinv("/", t->tokenv);
}

guint hc_token_len(HcTreeToken* t)
{
    return t->tokenc;
}

void hc_token_free(HcTreeToken* t)
{
    if (t == NULL)
        return;
    for (int i = 0; i < t->tokenc; i++)
        g_free(t->tokenv[i]);
    g_free(t);
}

/******************************************************************************
                                TREE
*****************************************************************************/

typedef struct __HTTPC_TREE__ {
    gchar* token;
    gpointer data;
    GList* children; // List of child nodes
} HttpcTree;

HttpcTree* hc_tree_new(gchar* token, gpointer data)
{
    HttpcTree* node = g_new(HttpcTree, 1);
    node->token = g_strdup(token);
    node->data = data;
    node->children = NULL;
    return node;
}

void hc_tree_add(HttpcTree* parent, HttpcTree* child)
{
    parent->children = g_list_append(parent->children, child);
}

void hc_tree_traverse(HttpcTree* root)
{
    // Process the data of the current node
    // ...

    // Traverse the child nodes recursively
    GList* iter = root->children;
    while (iter != NULL) {
        HttpcTree* child = iter->data;
        hc_tree_traverse(child);
        iter = g_list_next(iter);
    }
}

void hc_tree_free(HttpcTree* root)
{
    if (root == NULL)
        return;
    g_list_free_full(root->children, (GDestroyNotify)hc_tree_free);
    g_free(root);
}

gboolean hc_tree_insert(HttpcTree* parent, HcTreeToken* t, gpointer value, void (*free_fn)(gpointer mem), gboolean mkdir)
{
    if (parent == NULL || t == NULL || t->tokenv == NULL) {
        return FALSE;
    }
    // Traverse the tokens until the last one
    HttpcTree* current = parent;
    for (int i = 0; i < t->tokenc - 1; i++) {
        gboolean found = FALSE;
        // Check if a child node with the token already exists
        GList* iter = current->children;
        guint max_spin = g_list_length(iter);
        while (iter != NULL && max_spin-- > 0) {
            HttpcTree* child = iter->data;
            if (g_strcmp0(child->token, t->tokenv[i]) == 0) {
                current = child;
                found = TRUE;
                break;
            }
            iter = g_list_next(iter);
        }
        // If a child node doesn't exist and mkdir is enabled, create a new child node
        if (!found && mkdir) {
            HttpcTree* new = hc_tree_new(t->tokenv[i], NULL);
            current->children = g_list_append(current->children, new);
            current = new;
        } else if (!found && !mkdir) {
            // Return FALSE if a child node doesn't exist and mkdir is disabled
            return FALSE;
        }
    }

    // Create the last token's child node and set its value
    GList* iter = current->children;
    guint max_spin = g_list_length(iter);
    while (iter != NULL && max_spin-- > 0) {
        HttpcTree* child = iter->data;
        if (child != NULL && g_str_equal(child->token, t->tokenv[t->tokenc - 1])) {
            current->children = g_list_remove(current->children, child);
            if (free_fn != NULL)
                free_fn(child->data);
            g_free(child);
            HttpcTree* new = hc_tree_new(t->tokenv[t->tokenc - 1], value);
            current->children = g_list_append(current->children, new);
            return TRUE;
        }
    }
    HttpcTree* new = hc_tree_new(t->tokenv[t->tokenc - 1], value);
    current->children = g_list_append(current->children, new);
    return TRUE;
}

gpointer _hc_tree_get(HttpcTree* tree, HcTreeToken* t, guint _idx)
{
    /* Check if lookup index is out of bounds or if the current node's token
     * does not match */
    if (_idx >= t->tokenc || g_strcmp0(tree->token, t->tokenv[_idx]) != 0)
        return NULL;
    /* If this is the last token in the path, return the data associated with
     * the current node*/
    if (_idx == t->tokenc - 1)
        return tree->data;

    // Look for a child node with the next token in the path
    GList* iter = tree->children;
    while (iter != NULL) {
        HttpcTree* child = iter->data;
        gpointer result = _hc_tree_get(child, t, _idx + 1);
        if (result != NULL)
            return result; // Found a matching child node
        iter = g_list_next(iter);
    }

    return NULL; // No matching child node found
}

gpointer hc_tree_get(HttpcTree* tree, HcTreeToken* tree_token)
{
    return _hc_tree_get(tree, tree_token, 0);
}

void hc_tree_test()
{
    printf("START HTTPC-TREE TEST\n");
    // Create nodes
    HttpcTree* root = hc_tree_new("root", "hello");
    HttpcTree* node1 = hc_tree_new("node_1", "node_value1");
    HttpcTree* node2 = hc_tree_new("node_2", "node_value2");
    HttpcTree* node3 = hc_tree_new("node_3", "node_value3");

    // Build the tree structure
    hc_tree_add(root, node1);
    hc_tree_add(root, node2);
    hc_tree_add(node2, node3);
    HcTreeToken* t2 = hc_tokenize("root/node5/1/2/3/4");
    HcTreeToken* t3 = hc_tokenize("node5/1/2/3/4");
    gboolean done1 = hc_tree_insert(root, t3, "hello 2", NULL, FALSE);
    gboolean done2 = hc_tree_insert(root, t3, "hello 3", NULL, TRUE);
    char* v = (char*)hc_tree_get(root, t2);
    printf("hc_tree_insert#%s %d %d\n", v, done1, done2);

    gboolean done3 = hc_tree_insert(root, t3, "hello 5", NULL, FALSE);
    printf("hc_tree_insert#%s from %s %d\n", (char*)hc_tree_get(root, t2), v, done3);

    HcTreeToken* t = hc_tokenize("root/node_1");
    printf("%s", (char*)hc_tree_get(root, t));
    HcTreeToken* t_null = hc_tokenize("asdadsdsa");
    printf("%s", (char*)hc_tree_get(root, t_null));
    hc_token_free(t);
    hc_token_free(t_null);
    printf("END HTTPC-TREE TEST\n");
}
