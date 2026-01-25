#include <glib.h>
#include <stdio.h>

int main() {
    // Create balanced binary tree
    GTree *tree = g_tree_new((GCompareFunc)g_strcmp0);
    
    // Insert 3 elements
    g_tree_insert(tree, "apple", "red");
    g_tree_insert(tree, "banana", "yellow");
    g_tree_insert(tree, "grape", "purple");
    
    // Search for one element
    const char *color = g_tree_lookup(tree, "banana");
    
    if (color != NULL) {
        printf("banana is %s\n", color);
    } else {
        printf("banana not found\n");
    }
    
    // Clean up
    g_tree_destroy(tree);
    
    return 0;
}
