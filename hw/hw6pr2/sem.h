//
// Created by gary on 12/7/21.
//

#ifndef HW6PR2_SEM_H
#define HW6PR2_SEM_H

void sem_init(struct sem *s, int count);

int sem_try(struct sem *s);

void sem_wait(struct sem *s);

void sem_inc(struct sem *s);

#endif  // HW6PR2_SEM_H
