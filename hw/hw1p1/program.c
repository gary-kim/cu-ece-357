#include "program.h"

#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static char buf[4097];

int main(int argc, char** argv) {
  int outputfd = 1;
  int total = 0;
  int c;
  while ((c = getopt(argc, argv, "o:")) != -1) {
    switch (c) {
      case 'o':
        if (strcmp("-", optarg) == 0) {
          break;
        }
        outputfd = open(optarg, O_WRONLY | O_APPEND | O_TRUNC | O_CREAT, 0666);
        if (outputfd == -1) {
          fprintf(
              stderr,
              "Error while attempting to open output file \"%s\". errno = %i\n",
              optarg, errno);
          return -1;
        }
        break;
      case '?':
        if (optopt == 'o') {
          fprintf(stderr, "-o requires an argument\n");
        } else {
          fprintf(stderr, "Argument parsing failed");
        }
        return -1;
        break;
      default:
        return -1;
        break;
    }
  }
  if (optind == argc && (total = op(0, outputfd, "stdin")) < 0) {
    return -1;
  }
  for (int i = optind; i < argc; i++) {
    char* f = argv[i];
    int fd;
    if (strcmp(f, "-") == 0) {
      fd = 0;
    } else {
      fd = open(f, O_RDONLY);
    }
    if (fd == -1) {
      fprintf(stderr,
              "Error while attempting to open input file \"%s\". errno = %i\n",
              f, errno);
      return -1;
    }
    int done = op(fd, outputfd, f);
    if (done < 0) {
      return -1;
    }
    total += done;
  }
  int len = sprintf(buf, "%i\n", total);
  write_helper(2, buf, len);
  return 0;
}

int op(int fd, int outputfd, char* f) {
  int total = 0;
  int rc = 0;
  for (rc = read(fd, buf, 4096); rc > 0; rc = read(fd, buf, 4096)) {
    int err = write_helper(outputfd, buf, rc);
    if (err != 0) {
      fprintf(stderr, "Error while attempting to write to output. errno = %i",
              err);
      return -1;
    }
    total += rc;
  }
  if (rc == -1) {
    fprintf(stderr,
            "Error while attempting to read from input file \"%s\". errno = %i",
            f, errno);
    return -1;
  }
  return total;
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
