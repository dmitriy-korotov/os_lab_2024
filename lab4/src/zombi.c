#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    int status = 0;
    pid_t cpid = fork();
    if (cpid < 0) {
        perror("Can't fork process");
        exit(EXIT_FAILURE);
    }
    if (cpid == 0) {
        exit(EXIT_SUCCESS);
    }

    sleep(100);

    cpid = wait(&status);
    if (cpid == -1) {
        perror("Can't wait child process");
        exit(EXIT_FAILURE);
    }
    if (WIFEXITED(status)) {
        fprintf(stderr, "[%d]\tProcess %d exited with code %d",
                getpid(), cpid, WEXITSTATUS(status));
    }
    return 0;
}