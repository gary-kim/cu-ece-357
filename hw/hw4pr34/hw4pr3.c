#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <sys/signal.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
int write_helper(int fd, const void* buf, size_t count);

int main(int argc, char *argv[]){
    int pipefd[2];
    pid_t cpid;
    char buf;
    if (pipe2(pipefd, O_NONBLOCK) == -1) {
        fprintf(stderr, "error creating pipe: %s", strerror(errno));
        return -1;
    }
    // Contents of this array are not gauranteed but we don't actually
    // care what is in this string.
    char s[257];
    int error;
    if((error = write_helper(pipefd[1],&s,128))!=0){
        fprintf(stderr, "error with initial write: %s", strerror(error));
        return -1;
    }
    for(int i=128;;i+=256){
        if((error = write_helper(pipefd[1], &buf, 256))!=0){
            if (error == EAGAIN) {
                printf("The total size of the pipe is close to %i bytes\n", i+128);
                return 0;
            }
            fprintf(stderr, "error while writing to pipe: %s\n", strerror(error));
            return -1;
        }
        
    }
}

int write_helper(int fd, const void* buf, size_t count) {
  for (size_t written = 0; written < count;) {
    ssize_t r = write(fd, buf + written, count - written);
    if (r == -1) {
      return errno;
    }
    written += r;
  }
  return 0;
}