#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <pthread.h>

#include <errno.h>
#include <getopt.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <common.h>

struct Server {
  char ip[255];
  int port;
};

struct ClientArgs {
  struct Server server;
  uint64_t begin;
  uint64_t end;
  uint64_t mod;
};

bool ConvertStringToUI64(const char *str, uint64_t *val) {
  char *end = NULL;
  unsigned long long i = strtoull(str, &end, 10);
  if (errno == ERANGE) {
    fprintf(stderr, "Out of uint64_t range: %s\n", str);
    return false;
  }

  if (errno != 0)
    return false;

  *val = i;
  return true;
}

int GetServers(const char* path, struct Server** servers) {
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    fprintf(stderr, "Can't open file with servers\n");
    exit(EXIT_FAILURE);
  }

  const int max_servers_count = 10;

  struct Server buffer[max_servers_count];

  int idx = 0;
  while (true) {
    int count = fscanf(file, "%9s:%d\n", &buffer[idx].ip[0], &buffer[idx].port);
    printf("Count %d\n", count);
    if (count != 2) {
      break;
    }
    printf("Server %s:%d\n", &buffer[idx].ip[0], buffer[idx].port);
    idx++;
  }
  fclose(file);

  printf("Servers readed\n");

  *servers = (struct Server*)malloc(sizeof(struct Server) * idx);
  memcpy(*servers, buffer, sizeof(struct Server) * idx);
  return idx;
}

void* ServerHandler(void* args);

int main(int argc, char **argv) {
  uint64_t k = -1;
  uint64_t mod = -1;
  char servers[255] = {'\0'}; // TODO: explain why 255

  while (true) {
    int current_optind = optind ? optind : 1;

    static struct option options[] = {{"k", required_argument, 0, 0},
                                      {"mod", required_argument, 0, 0},
                                      {"servers", required_argument, 0, 0},
                                      {0, 0, 0, 0}};

    int option_index = 0;
    int c = getopt_long(argc, argv, "", options, &option_index);

    if (c == -1)
      break;

    switch (c) {
    case 0: {
      switch (option_index) {
      case 0:
        ConvertStringToUI64(optarg, &k);
        break;
      case 1:
        ConvertStringToUI64(optarg, &mod);
        break;
      case 2:
        memcpy(servers, optarg, strlen(optarg));
        break;
      default:
        printf("Index %d is out of options\n", option_index);
      }
    } break;

    case '?':
      printf("Arguments error\n");
      break;
    default:
      fprintf(stderr, "getopt returned character code 0%o?\n", c);
    }
  }

  if (k == -1 || mod == -1 || !strlen(servers)) {
    fprintf(stderr, "Using: %s --k 1000 --mod 5 --servers /path/to/file\n",
            argv[0]);
    return 1;
  }

  struct Server* servers_hosts;
  int servers_num = GetServers(servers, &servers_hosts);

  pthread_t threads[servers_num];
  struct ClientArgs args[servers_num];

  for (int i = 0; i < servers_num; i++) {
    args[i].begin = i * (k / servers_num) + 1;
    args[i].end = (i + 1) * (k / servers_num) + 1;
    if (i + 1 == servers_num) {
      args[i].end = k + 1;
    }
    args[i].mod = mod;
    args[i].server = servers_hosts[i];

    if (pthread_create(&threads[i], NULL, ServerHandler,
                        (void*)&args[i])) {
      printf("Error: pthread_create failed!\n");
      return EXIT_FAILURE;
    }
  }

  uint64_t total = 1;
  for (uint32_t i = 0; i < servers_num; i++) {
    uint64_t result = 0;
    pthread_join(threads[i], (void **)&result);
    total = MultModulo(total, result, mod);
  }
  free(servers_hosts);

  printf("Calculated factorial: %lu\n", total);

  return 0;
}

void* ServerHandler(void* args) {
  struct ClientArgs* client_args = (struct ClientArgs*)args;
  struct hostent *hostname = gethostbyname(client_args->server.ip);
  if (hostname == NULL) {
    fprintf(stderr, "gethostbyname failed with %s\n", client_args->server.ip);
    exit(1);
  }

  struct sockaddr_in server;
  server.sin_family = AF_INET;
  server.sin_port = htons(client_args->server.port);
  server.sin_addr.s_addr = *((unsigned long *)hostname->h_addr);

  int sck = socket(AF_INET, SOCK_STREAM, 0);
  if (sck < 0) {
    fprintf(stderr, "Socket creation failed!\n");
    exit(1);
  }

  if (connect(sck, (struct sockaddr *)&server, sizeof(server)) < 0) {
    fprintf(stderr, "Connection failed\n");
    exit(1);
  }

  char task[sizeof(uint64_t) * 3];
  memcpy(task, &client_args->begin, sizeof(uint64_t));
  memcpy(task + sizeof(uint64_t), &client_args->end, sizeof(uint64_t));
  memcpy(task + 2 * sizeof(uint64_t), &client_args->mod, sizeof(uint64_t));

  if (send(sck, task, sizeof(task), 0) < 0) {
    fprintf(stderr, "Send failed\n");
    exit(1);
  }

  char response[sizeof(uint64_t)];
  if (recv(sck, response, sizeof(response), 0) < 0) {
    fprintf(stderr, "Recieve failed\n");
    exit(1);
  }

  uint64_t answer = 0;
  memcpy(&answer, response, sizeof(uint64_t));
  printf("answer: %lu\n", answer);

  close(sck);
  return (void*)answer;
}
