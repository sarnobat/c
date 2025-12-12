#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdbool.h>

/* ANSI colors (matching git defaults) */
#define C_CYAN    "\033[36m"
#define C_YELLOW  "\033[33m"
#define C_MAGENTA "\033[35m"
#define C_GREEN   "\033[32m"
#define C_RED     "\033[31m"
#define C_RESET   "\033[0m"

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
    char *cmd[] = {
        "git", "log", "-1",
        "--date=short",
        "--pretty=format:%h %cd %s|%an",
        NULL
    };

    return git_cmd_capture(dir, cmd, buf, bufsz);
}

static void get_refs_at_head(const char *dir,
                             char *out,
                             size_t outsz) {
    out[0] = '\0';
    char buf[1024];
    bool first = true;

#define ADD_REF(r) do { \
    if (!first) strncat(out, ", ", outsz - strlen(out) - 1); \
    strncat(out, r, outsz - strlen(out) - 1); \
    first = false; \
} while (0)

    ADD_REF("HEAD");

    char *branches[] = {
        "git", "branch", "--all",
        "--points-at", "HEAD",
        "--format=%(refname:short)",
        NULL
    };

    if (git_cmd_capture(dir, branches, buf, sizeof(buf)) == 0) {
        char *line = strtok(buf, "\n");
        while (line) {
            ADD_REF(line);
            line = strtok(NULL, "\n");
        }
    }

    char *tags[] = {
        "git", "tag", "--points-at", "HEAD", NULL
    };

    if (git_cmd_capture(dir, tags, buf, sizeof(buf)) == 0) {
        char *line = strtok(buf, "\n");
        while (line) {
            ADD_REF(line);
            line = strtok(NULL, "\n");
        }
    }

#undef ADD_REF
}

static void get_unstaged_summary(const char *dir,
                                 int *m, int *a, int *u) {
    *m = *a = *u = 0;

    char buf[4096];
    char *cmd[] = {
        "git", "status", "--porcelain", NULL
    };

    if (git_cmd_capture(dir, cmd, buf, sizeof(buf)) != 0)
        return;

    for (char *line = strtok(buf, "\n"); line; line = strtok(NULL, "\n")) {
        if (line[0] == '?' && line[1] == '?') {
            (*u)++;
        } else if (line[1] == 'M') {
            (*m)++;
        } else if (line[1] == 'A') {
            (*a)++;
        }
    }
}

/* ------------------------------------------------------------ */

int main(int argc, char **argv) {
    bool long_mode = false;
    int path_cols = 50;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-l") == 0 ||
            strcmp(argv[i], "--long") == 0) {
            long_mode = true;
        } else if ((strcmp(argv[i], "-c") == 0 ||
                    strcmp(argv[i], "--columns") == 0) &&
                   i + 1 < argc) {
            path_cols = atoi(argv[++i]);
            if (path_cols <= 0)
                path_cols = 50;
        }
    }

    char dir[4096];

    while (fgets(dir, sizeof(dir), stdin)) {
        dir[strcspn(dir, "\n")] = 0;
        if (dir[0] == '\0')
            continue;

        if (!is_git_repo(dir))
            continue;

        int dirty = has_unstaged_changes(dir);
        int ahead = branch_ahead_of_upstream(dir);

        if (!dirty && !ahead)
            continue;

        if (!long_mode) {
            printf("%s\n", dir);
            continue;
        }

        char info[1024];
        if (get_last_commit(dir, info, sizeof(info)) != 0)
            continue;

        char hash[64], date[32];
        char *msg, *author;

        if (sscanf(info, "%63s %31s", hash, date) != 2)
            continue;

        msg = info + strlen(hash) + 1 + strlen(date) + 1;
        author = strchr(msg, '|');
        if (author) {
            *author = '\0';
            author++;
        } else {
            author = "";
        }

        char refs[1024];
        get_refs_at_head(dir, refs, sizeof(refs));

        printf("%-*s "
               C_CYAN "%s" C_RESET " "
               C_YELLOW "%s" C_RESET " "
               "%s "
               C_MAGENTA "%s" C_RESET " "
               C_GREEN "(%s)" C_RESET,
               path_cols, dir,
               hash, date, msg, author, refs);

        if (dirty) {
            int m, a, u;
            get_unstaged_summary(dir, &m, &a, &u);

            bool first = true;
            printf("  ");

            if (m > 0) {
                printf(C_RED "M" C_RESET " %d file%s",
                       m, m == 1 ? "" : "s");
                first = false;
            }
            if (a > 0) {
                if (!first) printf(", ");
                printf(C_RED "A" C_RESET " %d file%s",
                       a, a == 1 ? "" : "s");
                first = false;
            }
            if (u > 0) {
                if (!first) printf(", ");
                printf(C_RED "??" C_RESET " %d file%s",
                       u, u == 1 ? "" : "s");
            }
        }

        printf("\n");
    }

    return 0;
}
