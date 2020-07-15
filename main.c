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

#ifdef SERVER

static void* server(void* arg) {

  printf("%s started\n", __func__);

  int fd = socket(PF_INET, SOCK_STREAM, 6);

  {
    int val = 1;
    assert(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val)) == 0);
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

  char* buf = malloc(1024 * 1024);

  ssize_t nbytes = -1;

  printf("[server] Sending %d bytes...\n", 654954);
  nbytes = send(cfd, buf, 654954, MSG_NOSIGNAL);
  printf("[server] sent: %zd\n", nbytes);

  if (nbytes == -1) {
    perror("[server] send");
    assert(errno != EAGAIN);
    goto cleanup;
  }

  printf("[server] Sending %d bytes...\n", 573290);
  nbytes = send(cfd, buf, 573290, MSG_NOSIGNAL);
  printf("[server] sent: %zd\n", nbytes);

  if (nbytes == -1) {
    perror("[server] send");
    assert(errno != EAGAIN);
    goto cleanup;
  }

cleanup:
  free(buf);

  close(cfd);
  close(fd);

  return NULL;
}

#endif

#ifdef CLIENT

static void* client(void* arg) {

  printf("%s started\n", __func__);

  int fd = socket(PF_INET, SOCK_STREAM, 6);

  assert(fcntl(fd, F_SETFD, FD_CLOEXEC) == 0);

  struct sockaddr_in sa;
  sa.sin_family      = AF_INET;
  sa.sin_port        = PORT;
  sa.sin_addr.s_addr = HOST;

  if (connect(fd, (const struct sockaddr*)&sa, sizeof(sa)) != 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  char buf[1024];

  while (1) {

    while (recv(fd, buf, 1024, 0) > 0) {
      // do nothing
    }

    if (send(fd, buf, 1, 0) == -1) {
      exit(0);
    }

    usleep(250000);
  }

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
