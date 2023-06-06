#include "token_tree.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct __HTTPC_TREE_TOKEN__ {
    gchar* path;
    gchar** tokenv;
    guint tokenc;
} HcTreeToken;

typedef struct __HTTPC_TREE__ {
    gchar* token;
    gpointer data;
    GList* children; // List of child nodes
} HcTree;

/**
 * Checks whether given string matches with the token
 * @str   string pattern
 * @token to check
 * @return whether the token matches or not
 *
 * If string pattern is an HC_WILDCARD pattern, checks
 * whether the string is a valid value of that type
 *   *  HC_WILDCARD_FLOAT "{float}"
 *   *  HC_WILDCARD_INT "{int}"
 *   *  HC_WILDCARD "{}"
 *
 * If the string is not an HC_WILDCARD pattern it will
 * _match_pattern will simply check string equation
 */
gboolean _match_pattern(gchar* str, gchar* token);
/**
 * Recursively searches the tokens under the tree.
 * @_idx shows the depth of the recursion
 *
 * This function is used by the hc_tree_get.
 */
gpointer _hc_tree_get(HcTree* tree, HcTreeToken* t, guint _idx);

HcTreeToken* hc_tokenize(gchar* path)
{
    HcTreeToken* t = g_new(HcTreeToken, 1);
    t->path = strdup(path);
    t->tokenv = g_strsplit(path, "/", -1);
    t->tokenc = g_strv_length(t->tokenv);
    return t;
}

gchar* hc_token_str(HcTreeToken* t)
{
    return t->path;
}

guint hc_token_len(HcTreeToken* t)
{
    return t->tokenc;
}

void hc_token_free(HcTreeToken* t)
{
    if (t == NULL)
        return;
    g_free(t->path);
    for (guint i = 0; i < t->tokenc; i++)
        g_free(t->tokenv[i]);
    g_free(t);
}

HcTree* hc_tree_new(gchar* token, gpointer data)
{
    HcTree* node = g_new(HcTree, 1);
    node->token = g_strdup(token);
    node->data = data;
    node->children = NULL;
    return node;
}

void hc_tree_free(HcTree* root)
{
    if (root == NULL)
        return;
    g_list_free_full(root->children, (GDestroyNotify)hc_tree_free);
    g_free(root);
}

void hc_tree_add(HcTree* parent, HcTree* child)
{
    parent->children = g_list_append(parent->children, child);
}

gboolean hc_tree_insert(HcTree* parent, HcTreeToken* t, gpointer value,
    void (*free_fn)(gpointer mem), gboolean mkdir)
{
    if (parent == NULL || t == NULL || t->tokenv == NULL) {
        return FALSE;
    }
    // Traverse the tokens until the last one
    HcTree* current = parent;
    for (guint i = 0; i < t->tokenc - 1; i++) {
        gboolean found = FALSE;
        // Check if a child node with the token already exists
        GList* iter = current->children;
        guint max_spin = g_list_length(iter);
        while (iter != NULL && max_spin-- > 0) {
            HcTree* child = iter->data;
            if (g_strcmp0(child->token, t->tokenv[i]) == 0) {
                current = child;
                found = TRUE;
                break;
            }
            iter = g_list_next(iter);
        }
        // If a child node doesn't exist and mkdir is enabled, create new
        if (!found && mkdir) {
            HcTree* new = hc_tree_new(t->tokenv[i], NULL);
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
        HcTree* child = iter->data;
        if (child != NULL &&

            g_str_equal(child->token, t->tokenv[t->tokenc - 1])) {
            current->children = g_list_remove(current->children, child);
            if (free_fn != NULL)
                free_fn(child->data);
            g_free(child);
            HcTree* new = hc_tree_new(t->tokenv[t->tokenc - 1], value);
            current->children = g_list_append(current->children, new);
            return TRUE;
        }
    }
    HcTree* new = hc_tree_new(t->tokenv[t->tokenc - 1], value);
    current->children = g_list_append(current->children, new);
    return TRUE;
}

gpointer hc_tree_get(HcTree* tree, HcTreeToken* tree_token)
{
    gpointer p = _hc_tree_get(tree, tree_token, 0);
    return p;
}

gpointer _hc_tree_get(HcTree* tree, HcTreeToken* t, guint _idx)
{
    /* Check if lookup index is out of bounds or if the current node's token
     * does not match */
    if (_idx >= t->tokenc || !_match_pattern(tree->token, t->tokenv[_idx]))
        return NULL;
    /* If this is the last token in the path, return the data associated with
     * the current node*/
    if (_idx == t->tokenc - 1)
        return tree->data;
    // Look for a child node with the next token in the path
    GList* iter = tree->children;
    guint max_spin = g_list_length(iter);
    while (iter != NULL && max_spin-- > 0) {
        HcTree* child = iter->data;
        gpointer result = _hc_tree_get(child, t, _idx + 1);
        if (result != NULL)
            return result; // Found a matching child node
        iter = g_list_next(iter);
    }

    return NULL;
}

gboolean _match_pattern(gchar* str, gchar* token)
{
    char* endptr;
    if (g_str_equal(str, HC_WILDCARD_FLOAT)) {
        strtod(token, &endptr);
    } else if (g_str_equal(str, HC_WILDCARD_INT)) {
        strtol(token, &endptr, 10);
    } else {
        return g_str_equal(str, HC_WILDCARD) || g_str_equal(str, token);
    }
    return (*endptr == '\0');
}

void hc_tree_test()
{
    printf("START HTTPC-TREE TEST\n");
    // Create nodes
    HcTree* root = hc_tree_new("root", "hello");
    HcTree* node1 = hc_tree_new("node_1", "node_value1");
    HcTree* node2 = hc_tree_new("node_2", "node_value2");
    HcTree* node3 = hc_tree_new("node_3", "node_value3");
    HcTree* node4 = hc_tree_new("{}", "any_value");

    // Build the tree structure
    hc_tree_add(root, node1);
    hc_tree_add(root, node2);
    hc_tree_add(root, node4);
    hc_tree_add(node4, node3);
    HcTreeToken* t2 = hc_tokenize("root/node5/1/2/3/4");
    HcTreeToken* t3 = hc_tokenize("node5/1/2/3/4");
    gboolean done1 = hc_tree_insert(root, t3, "hello 2", NULL, FALSE);
    gboolean done2 = hc_tree_insert(root, t3, "hello 3", NULL, TRUE);
    char* v = (char*)hc_tree_get(root, t2);
    printf("hc_tree_insert#%s %d %d\n", v, done1, done2);

    gboolean done3 = hc_tree_insert(root, t3, "hello 5", NULL, FALSE);
    printf("hc_tree_insert#%s from %s %d\n", (char*)hc_tree_get(root, t2), v, done3);

    HcTreeToken* t = hc_tokenize("root/node_1");
    printf("%s\n", (char*)hc_tree_get(root, t));
    HcTreeToken* t_null = hc_tokenize("asdadsdsa");
    printf("%d\n", hc_tree_get(root, t_null) == NULL);
    hc_token_free(t);
    hc_token_free(t_null);

    printf("WILDCARD TEST\n");
    HcTreeToken* t_wc = hc_tokenize("root/node_4/node_3");
    char* t_wcptr = hc_tree_get(root, t_wc);
    printf("found %d %s\n", t_wcptr != NULL, t_wcptr);

    /* TODO: Add tests for other HC_WILDCARD cases */
    printf("END HTTPC-TREE TEST\n");
}
