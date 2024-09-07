#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <getopt.h>

#include "find_min_max.h"
#include "utils.h"

int main(int argc, char **argv) {
  int seed = -1;
  int array_size = -1;
  int pnum = -1;
  bool with_files = false;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"pnum", required_argument, 0, 0},
                                      {"by_files", no_argument, 0, 'f'},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "f", options, &option_index);

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

          defalut:
            printf("Index %d is out of options\n", option_index);
        }
        break;
      case 'f':
        with_files = true;
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

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);
  int active_child_processes = 0;

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  int chunk_size = array_size / pnum;

  int* pipes = (int*)malloc(pnum * sizeof(int) * 2);

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
      close(wd);
    } else {
      printf("Fork failed!\n");
      return 1;
    }
  }

  while (active_child_processes > 0) {
    pid_t cpid = wait(NULL);
    if (cpid == -1) {
      puts("Can't wait child process\n");
    }
    printf("Waited child process (pid = %i)\n", cpid);
    active_child_processes -= 1;
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

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(array);

  printf("Min: %d\n", min_max.min);
  printf("Max: %d\n", min_max.max);
  printf("Elapsed time: %fms\n", elapsed_time);
  fflush(NULL);
  return 0;
}
