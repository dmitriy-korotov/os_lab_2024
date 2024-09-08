#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, const char** argv) {
    pid_t cpid = fork();
    if (cpid < 0) {
        puts("Can't fork process\n");
    }
    if (cpid == 0) {
        static char* argv[] = { "sequential_min_max", "10", " 10000", NULL };
        execv("sequential_min_max", argv);
    }
    puts("Wait execv subprocess...\n");
    cpid = wait(NULL);
    if (cpid > 0) {
        puts("Execv subprocess waited!\n");
    }
    return EXIT_SUCCESS;
}