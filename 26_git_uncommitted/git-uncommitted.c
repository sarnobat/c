#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

/* ANSI colors (same palette as git log) */
#define C_CYAN   "\033[36m"
#define C_YELLOW "\033[33m"
#define C_RESET  "\033[0m"

/* ------------------------------------------------------------ */
/* helpers                                                      */
/* ------------------------------------------------------------ */

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

static int git_cmd_capture(const char *dir,
                           char *const argv[],
                           char *buf,
                           size_t bufsz) {
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

/* ------------------------------------------------------------ */
/* git predicates                                               */
/* ------------------------------------------------------------ */

static int is_git_repo(const char *dir) {
    char *cmd[] = {
        "git", "rev-parse", "--is-inside-work-tree", "--quiet", NULL
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
    char *cmd[] = {
        "git", "rev-list", "--left-right", "--count", "@{u}...HEAD", NULL
    };

    char buf[128];
    if (git_cmd_capture(dir, cmd, buf, sizeof(buf)) != 0)
        return 0;

    int behind = 0, ahead = 0;
    if (sscanf(buf, "%d %d", &behind, &ahead) != 2)
        return 0;

    return ahead > 0;
}

/* ------------------------------------------------------------ */
/* git info                                                     */
/* ------------------------------------------------------------ */

static int get_last_commit(const char *dir,
                           char *buf,
                           size_t bufsz) {
    /*
     * %h short hash
     * %cd committer date (short, YYYY-MM-DD)
     * %s subject
     */
    char *cmd[] = {
        "git", "log", "-1",
        "--date=short",
        "--pretty=format:%h %cd %s",
        NULL
    };

    return git_cmd_capture(dir, cmd, buf, bufsz);
}

/* ------------------------------------------------------------ */

int main(int argc, char **argv) {
    bool long_mode = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 ||
            strcmp(argv[i], "--long") == 0) {
            long_mode = true;
        }
    }

    char dir[4096];

    while (fgets(dir, sizeof(dir), stdin)) {
        dir[strcspn(dir, "\n")] = 0;
        if (dir[0] == '\0')
            continue;

        if (!is_git_repo(dir))
            continue;

        if (!has_unstaged_changes(dir) &&
            !branch_ahead_of_upstream(dir))
            continue;

        if (!long_mode) {
            printf("%s\n", dir);
        } else {
            char info[1024];
            if (get_last_commit(dir, info, sizeof(info)) == 0) {
                /*
                 * Split "<hash> <date> <message>"
                 */
                char hash[64], date[32];
                const char *msg = "";

                if (sscanf(info, "%63s %31s", hash, date) == 2) {
                    msg = info + strlen(hash) + 1 + strlen(date) + 1;
                }

                printf("%-60s "
                       C_CYAN "%s" C_RESET " "
                       C_YELLOW "%s" C_RESET " "
                       "%s\n",
                       dir, hash, date, msg);
            } else {
                printf("%-60s <no commits>\n", dir);
            }
        }
    }

    return 0;
}
