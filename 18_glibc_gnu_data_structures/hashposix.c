#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>

int main() {
    ENTRY item, *found;
    
    // Create hash table with space for at least 10 elements
    if (hcreate(10) == 0) {
        fprintf(stderr, "Failed to create hash table\n");
        return 1;
    }
    
    // Insert 3 elements (strdup to allocate on heap)
    item.key = strdup("apple");
    item.data = strdup("red");
    hsearch(item, ENTER);
    
    item.key = strdup("banana");
    item.data = strdup("yellow");
    hsearch(item, ENTER);
    
    item.key = strdup("grape");
    item.data = strdup("purple");
    hsearch(item, ENTER);
    
    // Query for one element
    item.key = "banana";
    found = hsearch(item, FIND);
    
    if (found != NULL) {
        printf("banana is %s\n", (char *)found->data);
    } else {
        printf("banana not found\n");
    }
    
    // Clean up
    hdestroy();
    
    return 0;
}

// Compile with: gcc -o hash_example hash_example.c