//
// Created by gary on 12/7/21.
//

#ifndef HW6PR2_SEM_H
#define HW6PR2_SEM_H

struct sem {
  int proc_num;
  int count;
  int *procs;
  char *locks_lock;
  unsigned int *locks;
  char *waiting_procs_lock;
  unsigned char *waiting_procs;
  unsigned int *sleep_procs;
  unsigned int *wake_procs;
};

void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);

int sem_signal_count();

void print_info(struct sem *s);

#endif  // HW6PR2_SEM_H
