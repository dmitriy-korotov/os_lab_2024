#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

double diffclock( clock_t clock1, clock_t clock2 ) {
    double diffticks = clock1 - clock2;
    double diffms = diffticks / ( CLOCKS_PER_SEC / 1000 );
    return diffms;
}

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;
  int timeout = -1;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {"timeout", required_argument, 0, 't'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "ft", options, &option_index);

    if (c == -1) break;

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            seed = atoi(optarg);
            if (seed < 0) {
              puts("Invalid seed value\n");
              return EXIT_FAILURE;
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size < 1) {
              puts("Invalid array_size value\n");
              return EXIT_FAILURE;
            }
            break;
          case 2:
            pnum = atoi(optarg);
            if (pnum < 1) {
              puts("Invalid pnum value\n");
              return EXIT_FAILURE;
            }
            break;
          case 3:
            with_files = true;
            break;
          case 4:
            if (strlen(optarg) < 1) {
              puts("Invalid timeout value\n");
              return EXIT_FAILURE;
            }
            timeout = atoi(optarg);
            if (timeout < 0) {
              puts("Invalid timeout value\n");
              return EXIT_FAILURE;
            }
            break;

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
        break;
      case 't':
        timeout = atoi(optarg);
        if (timeout < 0) {
          puts("Invalid timeout value\n");
          return EXIT_FAILURE;
        }
        break;

      case '?':
        break;

      default:
        printf("getopt returned character code 0%o?\n", c);
    }
  }

  if (optind < argc) {
    printf("Has at least one no option argument\n");
    return 1;
  }

  if (seed == -1 || array_size == -1 || pnum == -1) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --pnum \"num\" \n",
           argv[0]);
    return 1;
  }

  if (array_size < pnum) {
    puts("Pnum must be less then array_size\n");
    return EXIT_FAILURE;
  }

  printf("Args parsed! (array_size = %i pnum = %i)\n", array_size, pnum);
  fflush(NULL);


  printf("Start array generating...\n");
  fflush(NULL);
  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  printf("Array generated!\n");

  clock_t start_time = clock();

  int chunk_size = array_size / pnum;

  int* pipes = (int*)malloc(pnum * sizeof(int) * 2);
  pid_t* pids = (pid_t*)malloc(pnum * sizeof(pid_t));

  for (int i = 0; i < pnum; i++) {
    if (pipe(pipes + i * 2) == -1) {
      puts("Can't create pipe");
      return EXIT_FAILURE;
    }
    int rd = *(pipes + i * 2);
    int wd = *(pipes + i * 2 + 1);
    pid_t child_pid = fork();
    if (child_pid >= 0) {
      // successful fork
      active_child_processes += 1;
      if (child_pid == 0) {
        signal(SIGKILL, SIG_DFL);

        int chunk_begin = i * chunk_size;
        int chunk_end = chunk_begin + chunk_size;
        if (i + 1 == pnum) {
          chunk_end = array_size;
        }
        struct MinMax min_max = GetMinMax(array, chunk_begin, chunk_end);
        if (with_files) {
          char buff[128];
          sprintf(buff, "prcess_%i", i);
          FILE* file = fopen(buff, "w");
          if (!file) {
            printf("Can't open file (pid = %i)\n", child_pid);
            return EXIT_FAILURE;
          }
          if (fwrite(&min_max, sizeof(min_max), 1, file) != 1) {
            printf("Can't write min max (pid = %i)\n", child_pid);
            return EXIT_FAILURE;
          }
          fclose(file);
        } else {
          close(rd);
          if (write(wd, &min_max, sizeof(min_max)) == -1) {
            printf("Can't write min max (pid = %i)\n", child_pid);
            return EXIT_FAILURE;
          }
          close(wd);
        }
        return 0;
      }
      pids[i] = child_pid;
      printf("Forked process (cpid = %i)\n", child_pid);
      fflush(NULL);
      close(wd);
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  puts("Child processes created!");
  fflush(NULL);

  signal(SIGKILL, SIG_IGN);

  if (timeout != -1) {
    clock_t start_waiting = clock();
    while ((diffclock(clock(), start_time) < timeout) && active_child_processes > 0) {
      pid_t cpid = waitpid(0, NULL, WNOHANG);
      if (cpid == -1) {
        puts("Can't wait child process\n");
        return EXIT_FAILURE;
      }
      if (cpid > 0) {
        printf("Waited child process (pid = %i)\n", cpid);
        active_child_processes -= 1;
      }
    }

    for (int i = 0; i < pnum; i++) {
      int res = kill(pids[i], SIGKILL);
      if (res == -1) {
        printf("Can't send SIGKILL signal (pid = %i)\n", pids[i]);
        continue;
      }
      printf("Sended SIGKILL signal to child prcess (pid = %i)\n", pids[i]);
      active_child_processes -= 1;
    }
  } else {
    while (active_child_processes > 0) {
      pid_t cpid = waitpid(0, NULL, 0);
      if (cpid == -1) {
        puts("Can't wait child process\n");
        return EXIT_FAILURE;
      }
      printf("Waited child process (pid = %i)\n", cpid);
      active_child_processes -= 1;
    }
  }

  struct MinMax min_max;
  min_max.min = INT_MAX;
  min_max.max = INT_MIN;

  for (int i = 0; i < pnum; i++) {
    struct MinMax tmp_min_max;
    tmp_min_max.min = INT_MAX;
    tmp_min_max.max = INT_MIN;

    int rd = *(pipes + i * 2);
    int wd = *(pipes + i * 2 + 1);

    if (with_files) {
      char buff[128];
      sprintf(buff, "prcess_%i", i);
      FILE* file = fopen(buff, "r");
      if (!file) {
        printf("Can't open file (main process)\n");
        return EXIT_FAILURE;
      }
      if (fread(&tmp_min_max, sizeof(tmp_min_max), 1, file) != 1) {
        printf("Can't read min max (main process)\n");
        return EXIT_FAILURE;
      }
      fclose(file);
      remove(buff);
    } else {
      if (read(rd, &tmp_min_max, sizeof(tmp_min_max)) == -1) {
        printf("Can't write min max (main process)\n");
        return EXIT_FAILURE;
      }
      close(rd);
    }

    if (tmp_min_max.min < min_max.min) min_max.min = tmp_min_max.min;
    if (tmp_min_max.max > min_max.max) min_max.max = tmp_min_max.max;
  }

  clock_t finish_time = clock();

  double elapsed_time = diffclock(finish_time, start_time);

  free(array);
  free(pipes);
  free(pids);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
