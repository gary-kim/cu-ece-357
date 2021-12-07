//
// Created by gary on 12/7/21.
//

#ifdef USE_SPINLOCK

#include "spinlock.h"

#include "3rdparty/tas.h"

void spin_lock(volatile char *lock_state) {
  while (tas(lock_state) != 0)
    ;
}

void spin_unlock(volatile char *lock_state) { *lock_state = 0; }

#endif
