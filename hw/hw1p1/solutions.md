I am assuming for all of these problems that they are running on a Linux 5.13.12 system using GNU Libc 2.33

### Problem 1

a. Will always trigger a system call. Needs to call the `read` system call to be able to read a file on disk

b. Will sometimes trigger a system call. `malloc` adjusts the heap size using the `sbrk` or `mmap` system call depending on how much memory was requested. If the heap is already large enough, the syscall will not be made.

c. Will never trigger a system call. `sqrt` is a libc function that does a calculation without the help of the kernel

d. Will always trigger a system call. The clock_gettime syscall would be used to get the current time of day

### Problem 2

a. Tries to read 100 characters from the file descriptor `-1` into `buf` through the read system call (`buf` being a 10 char array). For this call an `EBADF` would be set on `errno` and `n` will be set to `-1`. This is because the file descriptor `-1` is invalid.

b. Calls the open system call which makes a new **open file description** set to write only mode, to create a file if it does not already exist, to truncate the file to zero length if it already exists. The kernel is also told that if the file is created, it should have `666` permissions which means read and write access for the owner, group, and world. fd is set to a **file descriptor** which references that **open file description**.

c. Creates a 3 char array called `buf`. Then, in a endless loop, tries to read 3 bytes from the file into `buf` using the `read` system call, then tries to print out the return of the `read` system call. The `read` system call returns the number of bytes written to `buf` so for the first 3 calls, it will return `3` and that will be printed to stdout. After 3 times, the read calls will continue but `EOF` has been reached so `read` will only return `0` and `0` will be printed out to stdout. This will continue forever.

d. Opens a file with the `open` system call at the location `/tmp/good` with flags for write only, append when writing, truncate if file already exists, create file with permission `666` if file does not exist. This returns  a file descriptor. Write into that file 6 bytes from `hewitt\n` (which would be just `hewitt`); After that, the write offset of the file descriptor is set to 0 using the `lseek` syscall. After that, the first 8 bytes of `8675309\n` is written to the end of the file. Even though the write offset was set back to the start of the file, because `O_APPEND` was set on the open command, the `write` syscall moves the offset back to the end of the file before writing to it. After that, the write offset is set to the end of the file with the `lseek` syscall and the return of the syscall, which is where the offset was set to, is set to `p` (in this case, this would be `14` as 14 bytes was written to the file before the offset was moved to the end of the file). `p` (`14`) is now printed out to stdout.