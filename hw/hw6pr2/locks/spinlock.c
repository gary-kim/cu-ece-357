//
// Created by gary on 12/7/21.
//

#if defined(USE_SPINLOCK) || defined(USE_SEM)

#include "spinlock.h"

#include "3rdparty/tas.h"

void spin_lock(volatile char *lock_state) {
  while (tas(lock_state) != 0)
    ;
}

int spin_try(volatile char* lock_state) {
  return tas(lock_state);
}

void spin_unlock(volatile char *lock_state) { *lock_state = 0; }

#endif
