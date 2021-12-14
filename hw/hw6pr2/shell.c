//
// Created by gary on 12/8/21.
//

#ifdef USE_SEM

#include "shell.h"

#define _GNU_SOURCE

#include <memory.h>
#include <stdio.h>
#include <sys/mman.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

#include "sem.h"

#define CUPS 3

int SHELLS_IN_CUP;
int NUMBER_OF_MOVES;
int my_proc_num = -1;

struct sem *s;
int s_num = CUPS;


struct config {
  int from_proc;
  int to_proc;
};

struct config *config;
int config_num = 0;

const char *HELP_MESSAGE =
    "%1$s - shell game with semaphores\n"
    "\n"
    "USAGE:\n"
    "\t%1$s count moves\n"
    "Example: %1$s 3 20000\n";


int main(int argc, char **argv) {
  if (argc < 2 || strstr(argv[1], "help") != NULL ||
      strcmp(argv[1], "h") == 0 || strcmp(argv[1], "-h") == 0) {
    printf(HELP_MESSAGE, argv[0]);
    return 0;
  }

  SHELLS_IN_CUP = (int) strtol(argv[1], NULL, 10);
  NUMBER_OF_MOVES = (int) strtol(argv[2], NULL, 10);
  if (SHELLS_IN_CUP == 0 || NUMBER_OF_MOVES == 0) {
    fprintf(stderr, "ERROR: given a zero or non-number argument\n");
    exit(255);
  }

  make_configs(CUPS);

  int *procs = mmap(NULL, sizeof(int) * CUPS, PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, 0, 0);
  if (procs == MAP_FAILED) {
    err(1, "getting a shared map region for procs array", errno);
  }

  for (int i = 0; i < s_num; i++) {
    s[i].proc_count = config_num;
    s[i].procs = procs;
    sem_init(&s[i], SHELLS_IN_CUP);
  }

  // Fork into processes
  for (int i = 0; i < config_num; i++) {
    int n = fork();
    if (n == -1) {
      err(1, "forking child processes", errno);
    }
    if (n == 0) {
      procs[i] = getpid();
      my_proc_num = i;
      child();
      exit(0);
    }
    procs[i] = n;
  }

  fprintf(stderr, "Main process spawned all children, waiting\n");

  // Wait until all processes return
  for (int i = 0; i < config_num; i++) {
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
    fprintf(stderr, "Child pid %i exited w/ 0\n", procs[i]);
  }

  // Report status of all
  fprintf(stderr,"Sem #      val        Sleeps     Wakes\n");
  for (int i = 0; i < s_num; i++) {
    fprintf(stderr,"%-10i%-10i\n", i, *s[i].locks);
    print_info(&s[i]);
  }
}

void err(int code, const char *where, int err) {
  fprintf(stderr, "ERROR: %s: %s\n", where, strerror(err));
  exit(code);
}

void child() {
  struct config c = config[my_proc_num];
  struct sem *from = &s[c.from_proc];
  struct sem *to = &s[c.to_proc];
  from->proc_num = my_proc_num;
  to->proc_num = my_proc_num;
  fprintf(stderr, "VCPU %i starting, pid %i\n", my_proc_num, s->procs[my_proc_num]);
  for (int i = 0; i < NUMBER_OF_MOVES; i++) {
    sem_wait(from);
    sem_inc(to);
  }
  fprintf(stderr, "Child %i (pid %i) done, signal handler was invoked %i times\nVCPU %i done\n", my_proc_num, s->procs[my_proc_num], sem_signal_count(), my_proc_num);
  exit(0);
}

void make_configs(int number_of_cpus) {
  s_num = number_of_cpus;
  s = malloc(sizeof(struct sem) * number_of_cpus);

  config_num = number_of_cpus * (number_of_cpus - 1);
  config = malloc(sizeof(struct config) * config_num);
  int i = 0;
  for (int j = 0; j < number_of_cpus; j++) {
    for (int k = 0; k < number_of_cpus; k++) {
      if (j == k) {
        continue;
      }
      config[i].from_proc = j;
      config[i].to_proc = k;
      i++;
    }
  }
}

#endif
