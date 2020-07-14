#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define PORT 57343
#define HOST (((((1 << 8) | 0) << 8) | 0) << 8) | 127

#define RCV_AND_LOG(id, fd, buf, buf_size)\
  (printf("[%s] Receiving %d bytes...\n", id, buf_size), nbytes = recv(fd, buf, buf_size, 0),            printf("[%s] received: %zd\n", id, nbytes))

#define SND_AND_LOG(id, fd, buf, buf_size)\
  (printf("[%s] Sending %d bytes...\n",   id, buf_size), nbytes = send(fd, buf, buf_size, MSG_NOSIGNAL), printf("[%s] sent: %zd\n",     id, nbytes))

#define SERVER_SLEEP_INTERVAL 100000
#define CLIENT_SLEEP_INTERVAL 150000

#ifdef SERVER

static void* server(void* arg) {

  printf("%s started\n", __func__);

  int fd = socket(PF_INET, SOCK_STREAM, 6);

  {
    int val = 1;
    assert(setsockopt(fd, 1, 2, &val, sizeof(val)) == 0);
    assert(setsockopt(fd, 6, 1, &val, sizeof(val)) == 0);
  }

  struct sockaddr_in sa;
  sa.sin_family      = AF_INET;
  sa.sin_port        = PORT;
  sa.sin_addr.s_addr = HOST;

  if (bind(fd, (const struct sockaddr*)&sa, sizeof(sa)) != 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  assert(listen(fd, 1) == 0);

  int cfd = accept(fd, NULL, NULL);
  printf("connection accepted\n");

  char buf[500000];

  ssize_t nbytes = -1;

  #include "server_seq.c"

  close(cfd);
  close(fd);

  return NULL;
}

#endif

#ifdef CLIENT

static void* client(void* arg) {
  printf("%s started\n", __func__);

  int fd = socket(PF_INET, SOCK_STREAM, 6);

  assert(fcntl(fd, 0x1, 0x6) == 0);
  assert(fcntl(fd, 0x2, 0x1) == 0);

  {
    int val = 1024 * 5;
    setsockopt(fd, SOL_SOCKET, SO_RCVBUF, &val, sizeof(val));
    setsockopt(fd, SOL_SOCKET, SO_SNDBUF, &val, sizeof(val));
  }

  struct sockaddr_in sa;
  sa.sin_family      = AF_INET;
  sa.sin_port        = PORT;
  sa.sin_addr.s_addr = HOST;

  if (connect(fd, (const struct sockaddr*)&sa, sizeof(sa)) != 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  char buf[500000];

  ssize_t nbytes = -1;

  #include "client_seq.c"

  close(fd);

  return NULL;
}

#endif

int main() {

#ifdef SERVER
  pthread_t st;
  pthread_create(&st, NULL, server, NULL);
#endif

#ifdef CLIENT
  pthread_t ct;
  pthread_create(&ct, NULL, client, NULL);
#else
  usleep(10000);
  system("./client");
#endif

  void* val;

#ifdef SERVER
  pthread_join(st, &val);
#endif

#ifdef CLIENT
  pthread_join(ct, &val);
#endif

  return 0;
}
