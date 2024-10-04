#include <errno.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <unistd.h>

void first_thread_handler(void*);
void second_thread_handler(void*);


pthread_mutex_t mut1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mut2 = PTHREAD_MUTEX_INITIALIZER;

int main() {
  pthread_t thread1, thread2;

  if (pthread_create(&thread1, NULL, (void *)first_thread_handler, NULL) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_create(&thread2, NULL, (void *)second_thread_handler, NULL) != 0) {
    perror("pthread_create");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(thread1, NULL) != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }

  if (pthread_join(thread2, NULL) != 0) {
    perror("pthread_join");
    exit(EXIT_FAILURE);
  }
  return EXIT_SUCCESS;
}

void first_thread_handler(void*) {
  while (true) {
    pthread_mutex_lock(&mut1);
    printf("[tid = %lu] Thread is alive!\n", pthread_self());
    pthread_mutex_lock(&mut2);

    pthread_mutex_unlock(&mut1);
    pthread_mutex_unlock(&mut2);
  }
}

void second_thread_handler(void*) {
  while (true) {
    pthread_mutex_lock(&mut2);
    printf("[tid = %lu] Thread is alive!\n", pthread_self());
    pthread_mutex_lock(&mut1);

    pthread_mutex_unlock(&mut2);
    pthread_mutex_unlock(&mut1);
  }
}
