#include <unistd.h>
#include <stdio.h>

int main(void) {
    char *argv[] = { "dash", "-c", "echo 'Hello from dash!'", NULL };
    execv("/tmp/dash/src/dash", argv);
    perror("execv failed");
    return 1;
}
