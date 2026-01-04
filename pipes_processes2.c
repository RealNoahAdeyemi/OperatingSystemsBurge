#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
// Generaated using You.com AI
/*
 * Usage: ./pipes_processes2 <pattern>
 * Example: ./pipes_processes2 28
 * This runs: cat scores | grep <pattern> | sort
 */

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pattern>\n", argv[0]);
        exit(1);
    }

    int pipefd1[2]; // cat -> grep
    int pipefd2[2]; // grep -> sort

    pipe(pipefd1);
    pipe(pipefd2);

    pid_t pid1 = fork();
    if (pid1 == 0) {
        // First child: will fork again for grep
        pid_t pid2 = fork();
        if (pid2 == 0) {
            // Grandchild: sort
            // stdin: pipefd2[0]
            dup2(pipefd2[0], STDIN_FILENO);
            close(pipefd2[1]);
            close(pipefd1[0]);
            close(pipefd1[1]);
            execlp("sort", "sort", (char *)NULL);
            perror("execlp sort");
            exit(1);
        } else {
            // Child: grep <pattern>
            // stdin: pipefd1[0], stdout: pipefd2[1]
            dup2(pipefd1[0], STDIN_FILENO);
            dup2(pipefd2[1], STDOUT_FILENO);
            close(pipefd1[1]);
            close(pipefd2[0]);
            execlp("grep", "grep", argv[1], (char *)NULL);
            perror("execlp grep");
            exit(1);
        }
    } else {
        // Parent: cat scores
        // stdout: pipefd1[1]
        dup2(pipefd1[1], STDOUT_FILENO);
        close(pipefd1[0]);
        close(pipefd2[0]);
        close(pipefd2[1]);
        execlp("cat", "cat", "scores", (char *)NULL);
        perror("execlp cat");
        exit(1);
    }
}
