#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "hello_sh.h"

// This runs before dash's main()
__attribute__((constructor))
static void setup_script(int argc, char **argv) {
    static char path[] = "/tmp/hello_embedXXXXXX";
    int fd = mkstemp(path);
    if (fd < 0) {
        perror("mkstemp");
        exit(1);
    }
    FILE *f = fdopen(fd, "w");
    fwrite(hello_sh, 1, hello_sh_len, f);
    fclose(f);

    // Rewrite argv to "dash <path>"
    static char *newargv[3];
    newargv[0] = "dash";
    newargv[1] = path;
    newargv[2] = NULL;

    // Replace the process's argv
    extern int main(int, char **);
    main(2, newargv);
    unlink(path);
    exit(0);
}
