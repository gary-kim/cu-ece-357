//
// Created by gary on 12/7/21.
//

#define _GNU_SOURCE

#include "test.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "spinlock.h"
#include "sem.h"

#define N_PROC 64
#define INCREMENT_TIMES 2048

int my_procnum = -1;
struct info {
  char lock;
  int important_number;
};

// Shared memory region
struct info *m;

int main(int argc, char **argv) {
  m = mmap(NULL, sizeof(struct info), PROT_READ | PROT_WRITE,
           MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (m == MAP_FAILED) {
    err(1, "getting a shared map region", errno);
  }
  int *procs = mmap(NULL, sizeof(int) * N_PROC, PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
  if (m == MAP_FAILED) {
    err(1, "getting a shared map region for procs array", errno);
  }

  struct sem s;
#ifdef USE_SEM
  s.count = N_PROC;
  s.procs = procs;
  sem_init(&s, 1);
#endif

  // Fork into processes
  for (int i = 0; i < N_PROC; i++) {
    int n = fork();
    if (n == -1) {
      err(1, "forking child processes", errno);
    }
    if (n == 0) {
      procs[i] = getpid();
      my_procnum = i;
      child(&s);
      exit(0);
    }
    procs[i] = n;
  }

  // Wait until all processes return
  for (int i = 0; i < N_PROC; i++) {
    int wstatus;
    if (waitpid(procs[i], &wstatus, 0) != procs[i]) {
      err(1, "unexpected return waiting for child process", errno);
    }
    if (WIFSIGNALED(wstatus)) {
      fprintf(stderr,
              "error: a child process has been killed by signal %i (%s)",
              WTERMSIG(wstatus), strsignal(wstatus));
      exit(1);
    }
    if (WEXITSTATUS(wstatus) != 0) {
      fprintf(stderr, "error from a child process: exited with %i\n",
              WEXITSTATUS(wstatus));
      exit(1);
    }
  }

  if (m->important_number == N_PROC * INCREMENT_TIMES) {
    printf("The value of important_number is %i as it should be\n",
           m->important_number);
    return 0;
  }
  printf("The value of important_number is %i but it should be %i\n",
         m->important_number, N_PROC * INCREMENT_TIMES);
  return -1;
}

void err(int code, const char *where, int err) {
  fprintf(stderr, "ERROR: %s: %s\n", where, strerror(err));
  exit(code);
}

void child(struct sem *s) {
  s->proc_num = my_procnum;
  for (int i = 0; i < INCREMENT_TIMES; i++) {
#ifdef USE_SPINLOCK
    spin_lock(&m->lock);
#endif
#ifdef USE_SEM
    sem_wait(s);
#endif
    m->important_number += 1;
#ifdef USE_SPINLOCK
    spin_unlock(&m->lock);
#endif
#ifdef USE_SEM
    sem_inc(s);
#endif
  }
  exit(0);
}
