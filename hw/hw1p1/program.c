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
#include <libgen.h>

static char buf[4096];
const char* help_message = " - concatenate and copy files\n"
  "\n"
  "USAGE:\n"
  "\tkitty [-o outfile] infile1 [...infile2...]\n"
  "\tkitty [-o outfile]\n";

int main(int argc, char** argv) {
  int outputfd = 1;
  int total = 0;
  int c;
  int help = 0;
  while ((c = getopt(argc, argv, "o:h")) != -1) {
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
      case 'h':
        help = 1;
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
  if (help || argc == 1) {
    char *progname = basename(argv[0]);
    int err = write_helper(1, progname, strlen(progname));
    if (err != 0) {
      fprintf(stderr, "Error while attempting to print help message. errno = %i\n", err);
      return -1;
    }
    err = write_helper(1, help_message, strlen(help_message));
    if (err != 0) {
      fprintf(stderr, "Error while attempting to print help message. errno = %i\n", err);
      return -1;
    }
    return 0;
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
  int err = write_helper(2, buf, len);
  if (err != 0) {
    // This is dumb. If writing to stderr with the kernel call does not work, there is no way the stdlib call will work.
    fprintf(stderr, "Error while attempting to print total characters written to stderr. errno = %i\n", err);
  }
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
