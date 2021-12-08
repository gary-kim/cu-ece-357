//
// Created by gary on 12/7/21.
//

#ifdef USE_SEM

#include "sem.h"

#define _GNU_SOURCE

#include <signal.h>
#include <stddef.h>
#include <sys/mman.h>

#include "spinlock.h"

void noop(int signal) {}

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
  void *locks = mmap(NULL,
                     sizeof(unsigned int) + sizeof(char) + sizeof(char) +
                         size_of_waiting_procs,
                     PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
  s->locks_lock = locks;
  s->locks = locks = locks + sizeof(char);
  s->waiting_procs_lock = locks = locks + sizeof(unsigned int);
  s->waiting_procs = locks + sizeof(char);

  (*s->locks) = count;

  // Register signal handler
  signal(SIGUSR1, noop);
}

int sem_try(struct sem *s) {
  if (spin_try(s->locks_lock) == 0) {
    if (*s->locks != 0) {
      (*s->locks)--;
      spin_unlock(s->locks_lock);
      return 1;
    }
    spin_unlock(s->locks_lock);
  }
  return 0;
}

void sem_wait(struct sem *s) {
  while (sem_try(s) == 0) {
    sigset_t nmask, omask, smask;
    sigemptyset(&nmask);
    sigemptyset(&smask);
    sigaddset(&nmask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &nmask, &omask);
    spin_lock(s->waiting_procs_lock);
    set_bit(s->waiting_procs, s->proc_num, 1);
    spin_unlock(s->waiting_procs_lock);
    sigsuspend(&smask);
    sigprocmask(SIG_SETMASK, &omask, NULL);
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

#endif
