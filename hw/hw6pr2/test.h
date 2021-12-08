//
// Created by gary on 12/7/21.
//

#ifndef HW6PR2_TEST_H
#define HW6PR2_TEST_H

#include "sem.h"

int main(int argc, char **argv);

void err(int code, const char *where, int err);

void child(struct sem *s);
#endif  // HW6PR2_TEST_H
