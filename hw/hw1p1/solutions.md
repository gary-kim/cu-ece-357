I am assuming for all of these problems that they are running on a Linux 5.13.12 system using GNU Libc 2.33

### Problem 1

a. Will always trigger a system call. Needs to call the `read` system call to be able to read a file on disk

b. Will sometimes trigger a system call. `malloc` adjusts the heap size using the `sbrk` or `mmap` system call depending on how much memory was requested. If the heap is already large enough, the syscall will not be made.

c. Will never trigger a system call. `sqrt` is a libc function that does a calculation without the help of the kernel

d. Will always trigger a system call. The clock_gettime syscall would be used to get the current time of day

### Problem 2

a. Tries to read 100 characters from the file descriptor `-1` into `buf`. For this call an `EBADF` would be set on `errno` and `n` will be set to `-1`. This is because the file descriptor `-1` is invalid.