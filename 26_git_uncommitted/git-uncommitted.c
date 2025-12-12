#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/*
 * Run a git command silently (stdout/stderr -> /dev/null).
 * Returns raw wait status.
 */
static int git_cmd_silent(const char *dir, char *const argv[]) {
    pid_t pid = fork();
    if (pid == 0) {
        if (chdir(dir) != 0)
            _exit(1);

        FILE *devnull = fopen("/dev/null", "w");
        if (devnull) {
            dup2(fileno(devnull), STDOUT_FILENO);
            dup2(fileno(devnull), STDERR_FILENO);
            fclose(devnull);
        }

        execvp("git", argv);
        _exit(1);
    }

    int status;
    waitpid(pid, &status, 0);
    return status;
}

/*
 * Run a git command and capture stdout into buf.
 * buf is always NUL-terminated on success.
 * Returns 0 on success, nonzero otherwise.
 */
static int git_cmd_capture(const char *dir, char *const argv[],
                           char *buf, size_t bufsz) {
    int pipefd[2];
    if (pipe(pipefd) != 0)
        return 1;

    pid_t pid = fork();
    if (pid == 0) {
        if (chdir(dir) != 0)
            _exit(1);

        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        FILE *devnull = fopen("/dev/null", "w");
        if (devnull) {
            dup2(fileno(devnull), STDERR_FILENO);
            fclose(devnull);
        }

        execvp("git", argv);
        _exit(1);
    }

    close(pipefd[1]);
    ssize_t n = read(pipefd[0], buf, bufsz - 1);
    close(pipefd[0]);

    int status;
    waitpid(pid, &status, 0);

    if (n < 0)
        return 1;

    buf[n] = '\0';
    return (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? 0 : 1;
}

static int is_git_repo(const char *dir) {
    char *cmd[] = {
        "git", "rev-parse", "--is-inside-work-tree", NULL
    };
    int st = git_cmd_silent(dir, cmd);
    return WIFEXITED(st) && WEXITSTATUS(st) == 0;
}

static int has_unstaged_changes(const char *dir) {
    char *cmd[] = {
        "git", "diff", "--quiet", NULL
    };
    int st = git_cmd_silent(dir, cmd);
    return WIFEXITED(st) && WEXITSTATUS(st) == 1;
}

static int branch_ahead_of_upstream(const char *dir) {
    /*
     * git rev-list --left-right --count @{u}...HEAD
     * Output: "<behind> <ahead>\n"
     */
    char *cmd[] = {
        "git", "rev-list", "--left-right", "--count", "@{u}...HEAD", NULL
    };

    char buf[128];
    if (git_cmd_capture(dir, cmd, buf, sizeof(buf)) != 0)
        return 0; /* no upstream or error */

    int behind = 0, ahead = 0;
    if (sscanf(buf, "%d %d", &behind, &ahead) != 2)
        return 0;

    return ahead > 0;
}

int main(void) {
    char dir[4096];

    while (fgets(dir, sizeof(dir), stdin)) {
        dir[strcspn(dir, "\n")] = 0;
        if (dir[0] == '\0')
            continue;

        if (!is_git_repo(dir))
            continue;

        if (has_unstaged_changes(dir) ||
            branch_ahead_of_upstream(dir)) {
            printf("%s\n", dir);
        }
    }

    return 0;
}
