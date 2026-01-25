#define _GNU_SOURCE
#include <sys/queue.h>
#include <stdio.h>
#include <stdlib.h>

struct entry {
    char *str;
    TAILQ_ENTRY(entry) links;
};
TAILQ_HEAD(listhead, entry);

int main(void) {
    struct listhead list = TAILQ_HEAD_INITIALIZER(list);
    struct entry *e;

    // prepend(list, "one")
    e = malloc(sizeof *e); e->str = "one";   TAILQ_INSERT_HEAD(&list, e, links);

    // prepend(list, "two")
    e = malloc(sizeof *e); e->str = "two";   TAILQ_INSERT_HEAD(&list, e, links);

    // prepend(list, "three")
    e = malloc(sizeof *e); e->str = "three"; TAILQ_INSERT_HEAD(&list, e, links);

    // print all
    TAILQ_FOREACH(e, &list, links)
        printf("%s\n", e->str);
}
