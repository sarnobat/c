
#include <stdio.h>
#include <search.h>

typedef struct Node {
    struct Node *next;
    struct Node *prev;
    char *data;
} Node;

int main() {
    Node n1 = {NULL, NULL, "first"};
    Node n2 = {NULL, NULL, "second"};
    Node n3 = {NULL, NULL, "third"};
    
    insque(&n2, &n1);
    insque(&n3, &n2);
    
    // Dequeue from head
    printf("Dequeued: %s\n", n1.data);
    remque(&n1);
    
    return 0;
}