#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>

const char *HELP_MESSAGE =
    "%1$s - test mmap features\n"
    "\n"
    "USAGE:\n"
    "\t%1$s test_number\n";

void handler(int sig) {
  printf("Signal [%i: %s] received\n", sig, strsignal(sig));
  exit(sig);
}

// From hw1pr3 programming assignment
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

int test1() {
  printf("Executing Test #1 (write to r/o mmap):\n");
  char *map = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ, MAP_ANONYMOUS|MAP_PRIVATE, 0, 0);
  // TODO: should we set this to something before?
  printf("map[3] == '%c'\n", map[3]);
  char c = 'B';
  if (c == map[3]) {
    c = 'A';
  }
  printf("writing a '%c'\n", c);
  map[3] = c;
  return c == map[3];
}

int mktempfile() {
  char template[] = "/tmp/gjmtest-XXXXXX";
  int fd = mkstemp(template);
  unlink(template);
  return fd;
}

int test2() {
  printf("Executing Test #2 (write to MAP_SHARED):\n");
  int fd = mktempfile();
  write_helper(fd, "1234567890", 10);
  char *map = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
  printf("map[3] == '%c'\n", map[3]);
  char c = 'B';
  printf("writing a '%c'\n", c);
  map[3] = c;
  printf("checking if '%c' can also be read via the read(2) syscall\n", map[3]);
  lseek(fd, 3, SEEK_SET);
  char buf[10];
  read(fd, buf, 1);
  if (c == buf[0]) {
    printf("written value visible\n");
    return 0;
  } else {
    printf("written value is not visible\n");
    return 1;
  }
}

int test3() {
  printf("Executing Test #3 (write to MAP_PRIVATE):\n");
  int fd = mktempfile();
  write_helper(fd, "1234567890", 10);
  char *map = mmap(NULL, sysconf(_SC_PAGE_SIZE), PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
  printf("map[3] == '%c'\n", map[3]);
  char c = 'B';
  printf("writing a '%c'\n", c);
  map[3] = c;
  printf("checking if '%c' can also be read via the read(2) syscall\n", map[3]);
  lseek(fd, 3, SEEK_SET);
  char buf[10];
  read(fd, buf, 1);
  if (c == buf[0]) {
    printf("written value visible\n");
    return 0;
  } else {
    printf("written value is not visible\n");
    return 1;
  }
}

int main(int argc, char **argv) {
  if (argc < 2 || strstr(argv[1], "help") != NULL ||
      strcmp(argv[1], "h") == 0 || strcmp(argv[1], "-h") == 0) {
    printf(HELP_MESSAGE, argv[0]);
    return 0;
  }

  struct sigaction sa;
  sa.sa_handler = handler;
  sigemptyset(&sa.sa_mask);

  // Set all signal handlers possible
  for (int i = 0; i < SIGRTMIN; i++) {
    // This may fail but if it does, assume that the signal handler cannot be set for that signal.
    sigaction(i, &sa, NULL);
  }

  // Test 1
  if (argv[1][0] == '1') {
    if (test1()) {
      return 0;
    }
    return 255;
  }

  // Test 2
  if (argv[1][0] == '2') {
    return test2();
  }

  // Test 3
  if (argv[1][0] == '3') {
    return test3();
  }

  printf("Did not recognize given test number. Run \"%s help\" for usage information\n", argv[0]);
  return 1;
}
