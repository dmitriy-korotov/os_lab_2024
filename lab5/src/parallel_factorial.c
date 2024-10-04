#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>

#include <pthread.h>

#include <sys/time.h>

struct SharedData {
    uint64_t* factorial;
    pthread_mutex_t* mutex;
    size_t begin;
    size_t end;
    uint64_t mod;
};

void *ThreadFactorial(void *args) {
  struct SharedData *calculation_args = (struct SharedData*)args;
  for (; calculation_args->begin < calculation_args->end; calculation_args->begin++) {
    pthread_mutex_lock(calculation_args->mutex);
    *calculation_args->factorial *= calculation_args->begin;
    *calculation_args->factorial %= calculation_args->mod;
    pthread_mutex_unlock(calculation_args->mutex);
  }
  return NULL;
}

int main(int argc, char **argv) {
  uint32_t k = 0;
  uint32_t tnum = 0;
  uint32_t mod = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"tnum", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "ft", options, &option_index);

    if (c == -1) {
      break;
    }

    switch (c) {
      case 0:
        switch (option_index) {
          case 0:
            k = atoi(optarg);
            if (k < 0) {
              perror("Invalid k value");
              exit(EXIT_FAILURE);
            }
            break;
          case 1:
            tnum = atoi(optarg);
            if (tnum < 1) {
              perror("Invalid tnum value\n");
              exit(EXIT_FAILURE);
            }
            break;
          case 2:
            mod = atoi(optarg);
            if (mod < 1) {
              perror("Invalid mod value\n");
              exit(EXIT_FAILURE);
            }
            break;
          defalut:
            printf("Index %d is out of options\n", option_index);
            exit(EXIT_FAILURE);
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
    exit(EXIT_FAILURE);
  }

  if (tnum == 0 || mod == 0) {
    printf("Usage: %s -k \"num\" --pnum \"num\" --mod \"num\" \n",
           argv[0]);
    exit(EXIT_FAILURE);
  }

  if (k < tnum) {
    printf("Args 'k' must be grater then 'tnum'");
    exit(EXIT_FAILURE);
  }

  uint64_t factorial = 1;
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

  pthread_t* threads = (pthread_t*)malloc(tnum * sizeof(pthread_t));
  struct SharedData* args = (struct SharedData*)malloc(tnum * sizeof(struct SharedData));

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (uint32_t i = 0; i < tnum; i++) {
    struct SharedData sharedData;
    sharedData.factorial = &factorial;
    sharedData.mutex = &mutex;
    sharedData.begin = i * (k / tnum) + 1;
    sharedData.end = (i + 1) * (k / tnum) + 1;
    if (i + 1 == tnum) {
      sharedData.end = k + 1;
    }
    sharedData.mod = mod;
    args[i] = sharedData;
    if (pthread_create(&threads[i], NULL, ThreadFactorial, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      exit(EXIT_FAILURE);
    }
  }

  printf("All threads started!\n");

  for (uint32_t i = 0; i < tnum; i++) {
    pthread_join(threads[i], NULL);
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;

  free(threads);
  free(args);
  printf("Calculated factorial: %lu\n", factorial);
  printf("Elapsed time: %lfms\n", elapsed_time);
  return 0;
}
