#include <glib.h>
#include <stdio.h>

int main() {
    // Create hash table with string keys and values
    GHashTable *hash = g_hash_table_new(g_str_hash, g_str_equal);
    
    // Insert 3 elements
    g_hash_table_insert(hash, "apple", "red");
    g_hash_table_insert(hash, "banana", "yellow");
    g_hash_table_insert(hash, "grape", "purple");
    
    // Query for one element
    const char *color = g_hash_table_lookup(hash, "banana");
    
    if (color != NULL) {
        printf("banana is %s\n", color);
    } else {
        printf("banana not found\n");
    }
    
    // Clean up
    g_hash_table_destroy(hash);
    
    return 0;
}

// Compile with: gcc -o hash_example hash_example.c `pkg-config --cflags --libs glib-2.0`