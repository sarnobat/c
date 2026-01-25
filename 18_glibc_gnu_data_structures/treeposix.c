#include <stdio.h>
#include <stdlib.h>
#include <search.h>
#include <string.h>

int main() {
    void *root = NULL;
    
    // Insert 3 elements
    char *apple = "apple";
    char *banana = "banana";
    char *grape = "grape";
    
    tsearch(&apple, &root, (int (*)(const void *, const void *))strcmp);
    tsearch(&banana, &root, (int (*)(const void *, const void *))strcmp);
    tsearch(&grape, &root, (int (*)(const void *, const void *))strcmp);
    
    // Search for one element
    char *key = "banana";
    void **result = tfind(&key, &root, (int (*)(const void *, const void *))strcmp);
    
    if (result != NULL) {
        printf("Found: %s\n", **(char ***)result);
    } else {
        printf("Not found\n");
    }
    
    return 0;
}
