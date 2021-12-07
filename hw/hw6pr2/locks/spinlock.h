//
// Created by gary on 12/7/21.
//

#ifndef HW6PR2_SPINLOCK_H
#define HW6PR2_SPINLOCK_H

void spin_lock(volatile char *lock_state);

void spin_unlock(volatile char *lock_state);

#endif //HW6PR2_SPINLOCK_H
