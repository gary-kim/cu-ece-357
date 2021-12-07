//
// Created by gary on 12/7/21.
//

#ifdef USE_SEM

#include "sem.h"

#define _GNU_SOURCE

#include <sys/mman.h>

// Set a specific bit in the procs
// Yes, I used this function to mess with bitwise stuff too :)
void set_bit(unsigned char *procs, unsigned int index, int to) {
  unsigned int i = index >> 3;
  unsigned int r = (index & 0x7);
  unsigned char t = ((unsigned char) 0x80) >> r;
  if (to) {
    procs[i] = procs[i] | t;
  } else {
    procs[i] = procs[i] & (~t);
  }
}

void sem_init(struct sem *s, unsigned int count) {
  s->locks = count;
  s->waitingProcs = mmap(NULL, (count >> 3) + (((count & 0x7) == 0x0) ? 0 : 1), PROT_READ|PROT_WRITE, MAP_SHARED|MAP_ANONYMOUS, 0, 0);
}

void sem_try(struct sem *s) {

}

#endif
