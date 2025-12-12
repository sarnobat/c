#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

static int git_cmd(const char *dir, char *const argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        if (chdir(dir) != 0)
            _exit(1);
        execvp("git", argv);
        _exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    return status;
}

int main(void) {
    char dir[4096];

    while (fgets(dir, sizeof(dir), stdin)) {
        /* strip newline */
        dir[strcspn(dir, "\n")] = 0;
        if (dir[0] == '\0')
            continue;

        /* is it a git repo? */
        char *is_repo[] = {
            "git", "rev-parse", "--is-inside-work-tree", NULL
        };

        if (git_cmd(dir, is_repo) != 0)
            continue;

        /*
         * unstaged changes only
         * exit code 1 => unstaged changes exist
         */
        char *unstaged[] = {
            "git", "diff", "--quiet", NULL
        };

        int st = git_cmd(dir, unstaged);

        if (WIFEXITED(st) && WEXITSTATUS(st) == 1) {
            printf("%s\n", dir);
        }
    }

    return 0;
}
