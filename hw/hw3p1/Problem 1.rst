a)
    Program provided in the `Program 1A.c <./Program 1A.c>`_.

b)
    There may have been a program in the current working directory called "foo" that ran instead which ran the `/usr/bin/foo` program using the `execl` call, giving it a argv array that had `argv[0] = "./bar"`.
c)
    The registers are not shared between the different threads.

    The stack section of the memory space is also not shared between the different threads.
