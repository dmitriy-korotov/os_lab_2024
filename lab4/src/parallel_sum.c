#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <getopt.h>
#include <time.h>

#include <pthread.h>

#include <sys/time.h>

#include <utils.h>
#include <sum.h>

void *ThreadSum(void *args) {
  struct SumArgs *sum_args = (struct SumArgs *)args;
  return (void *)(int64_t)Sum(sum_args);
}

int main(int argc, char **argv) {
  uint32_t threads_num = 0;
  uint32_t array_size = 0;
  uint32_t seed = 0;

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"seed", required_argument, 0, 0},
                                      {"array_size", required_argument, 0, 0},
                                      {"threads_num", required_argument, 0, 0},
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
            seed = atoi(optarg);
            if (seed < 1) {
              perror("Invalid seed value");
              exit(EXIT_FAILURE);
            }
            break;
          case 1:
            array_size = atoi(optarg);
            if (array_size < 1) {
              perror("Invalid array_size value\n");
              exit(EXIT_FAILURE);
            }
            break;
          case 2:
            threads_num = atoi(optarg);
            if (threads_num < 1) {
              perror("Invalid threads_num value\n");
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

  if (seed == 0 || array_size == 0 || threads_num == 0) {
    printf("Usage: %s --seed \"num\" --array_size \"num\" --threads_num \"num\" \n",
           argv[0]);
    exit(EXIT_FAILURE);
  }

  if (array_size < threads_num) {
    puts("Threads_num must be less then array_size\n");
    exit(EXIT_FAILURE);
  }

  pthread_t* threads = (pthread_t*)malloc(threads_num * sizeof(pthread_t));

  int *array = malloc(sizeof(int) * array_size);
  GenerateArray(array, array_size, seed);

  struct SumArgs* args = (struct SumArgs*)malloc(threads_num * sizeof(struct SumArgs));

  struct timeval start_time;
  gettimeofday(&start_time, NULL);

  for (uint32_t i = 0; i < threads_num; i++) {
    struct SumArgs args_for_thread;
    args->array = array;
    args->begin = i * (array_size) / threads_num;
    args->end = (i + 1) * (array_size) / threads_num;
    if (i + 1 == threads_num) {
      args->end = array_size;
    }
    args[i] = args_for_thread;
    if (pthread_create(&threads[i], NULL, ThreadSum, (void *)&args[i])) {
      printf("Error: pthread_create failed!\n");
      exit(EXIT_FAILURE);
    }
  }

  printf("All threads started!\n");

  int total_sum = 0;
  for (uint32_t i = 0; i < threads_num; i++) {
    int sum = 0;
    pthread_join(threads[i], (void **)&sum);
    total_sum += sum;
  }

  struct timeval finish_time;
  gettimeofday(&finish_time, NULL);

  double elapsed_time = (finish_time.tv_sec - start_time.tv_sec) * 1000.0;
  elapsed_time += (finish_time.tv_usec - start_time.tv_usec) / 1000.0;
  
  free(array);
  free(threads);
  free(args);
  printf("Total sum: %d\n", total_sum);
  printf("Elapsed time: %lfms\n", elapsed_time);
  return 0;
}
