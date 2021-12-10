//
// Created by gary on 12/7/21.
//

#ifdef USE_SEM

#include "sem.h"

#define _GNU_SOURCE

#include <signal.h>
#include <stddef.h>
#include <sys/mman.h>
#include <stdio.h>

#include "spinlock.h"

int signum;

void noop(int signal) {
  // Should not cause a race condition as each task is single-threaded and
  // there is no way for this signal to be called while this signal is being
  // run as this signal handler is already running since SA_NODEFER is not set.
  signum++;
}

// Set a specific bit in the procs
// Yes, I used this function to mess with bitwise stuff too :)
void set_bit(unsigned char *procs, unsigned int index, int to) {
  unsigned int i = index >> 3;
  unsigned int r = (index & 0x7);
  unsigned char t = ((unsigned char)0x80) >> r;
  if (to) {
    procs[i] = procs[i] | t;
  } else {
    procs[i] = procs[i] & (~t);
  }
}

int get_bit(const unsigned char *procs, unsigned int index) {
  unsigned int i = index >> 3;
  unsigned int r = (index & 0x7);
  unsigned char t = ((unsigned char)0x80) >> r;
  int tr = ((procs[i] & t) == t);
  return tr;
}

void sem_init(struct sem *s, int count) {
  unsigned int c = s->count;
  unsigned int size_of_waiting_procs = (c >> 3) + (((c & 0x7) == 0x0) ? 0 : 1);
  unsigned int size_of_sleep_procs = c * sizeof(unsigned int);
  unsigned int size_of_wake_procs = c * sizeof(unsigned int);
  char *locks = mmap(NULL,
                     sizeof(unsigned int) + (2 * sizeof(char)) +
                         size_of_waiting_procs + size_of_sleep_procs + size_of_wake_procs,
                     PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
  s->locks_lock = locks;
  s->locks = (unsigned int *) (locks = locks + sizeof(char));
  s->waiting_procs_lock = locks = locks + sizeof(unsigned int);
  s->waiting_procs = (unsigned char *) (locks = locks + sizeof(char));
  s->sleep_procs = (unsigned int *) (locks = locks + size_of_waiting_procs);
  s->wake_procs = (unsigned int *) (locks + size_of_sleep_procs);

  (*s->locks) = count;

  // Register signal handler
  struct sigaction sa;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sa.sa_handler = noop;
  sigaction(SIGUSR1, &sa, NULL);
  //signal(SIGUSR1, noop);
}

int sem_try(struct sem *s) {
  spin_lock(s->locks_lock);
  if ((*s->locks) > 0) {
    (*s->locks)--;
    spin_unlock(s->locks_lock);
    return 1;
  }
  spin_unlock(s->locks_lock);
  return 0;
}

void sem_wait(struct sem *s) {
  while (sem_try(s) == 0) {
    sigset_t mask, suspend_mask;
    sigemptyset(&mask);
    sigemptyset(&suspend_mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // Record going to sleep
    s->sleep_procs[s->proc_num]++;

    spin_lock(s->waiting_procs_lock);
    set_bit(s->waiting_procs, s->proc_num, 1);
    spin_unlock(s->waiting_procs_lock);
    sigsuspend(&suspend_mask);

    // Record being awoken
    s->wake_procs[s->proc_num]++;

    sigprocmask(SIG_UNBLOCK, &mask, NULL);
  }
}

void sem_inc(struct sem *s) {
  spin_lock(s->locks_lock);
  (*s->locks)++;
  spin_unlock(s->locks_lock);
  spin_lock(s->waiting_procs_lock);
  for (int i = 0; i < s->count; i++) {
    if (get_bit(s->waiting_procs, i) && i != s->proc_num) {
      set_bit(s->waiting_procs, i, 0);
      kill(s->procs[i], SIGUSR1);
    }
  }
  spin_unlock(s->waiting_procs_lock);
}

int sem_signal_count() {
    return signum;
};

void print_info(struct sem *s) {
  for (int i = 0; i < s->count; i++) {
    fprintf(stderr," VCPU %-4i         %9i %9i\n", i, s->sleep_procs[i], s->wake_procs[i]);
  }
}

#endif
