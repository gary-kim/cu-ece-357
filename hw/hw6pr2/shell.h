//
// Created by gary on 12/8/21.
//

#ifndef HW6PR2_SHELL_H
#define HW6PR2_SHELL_H

int main(int argc, char **argv);

void err(int code, const char *where, int err);

void child();

void make_configs(int number_of_cpus);

#endif  // HW6PR2_SHELL_H
