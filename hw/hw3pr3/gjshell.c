#include <alloca.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int status = 0;

struct exitInfo {
  int wstatus;
  struct rusage usage;
  int errnum;
  pid_t pid;
};

struct programInfo {
  int stdin;
  int stdout;
  int stderr;
  char *path;
  char **argv;
  int argc;
  int errnum;
};

struct programInfo tokenizer(char *input) {
  const char *delimiter = " ";
  char *token = strtok(input, delimiter);
  struct programInfo pi = {0, 1, 2, "", NULL, 1};
  if (token == NULL) {
    pi.errnum = -1;
    return pi;
  }
  pi.path = malloc(strlen(token) + 1);
  strcpy(pi.path, token);
  pi.argv = malloc(1 * sizeof(char *));
  pi.argv[0] = pi.path;

  token = strtok(NULL, delimiter);
  while (token != NULL) {
    if (token[0] == '>' && token[1] == '>') {  // for >> redirection
      if (strlen(token) != 2) {
        token += 2;
      } else {
        token = strtok(NULL, delimiter);
      }
      pi.stdout = open(token, O_CREAT | O_APPEND | O_WRONLY, 0664);
      if (pi.stdout == -1) {
        pi.errnum = errno;
        fprintf(stderr, "Error opening file: %s, err: %s\n", token,
                strerror(errno));
        return pi;
      }
    } else if (token[0] == '2' && token[1] == '>' &&
               token[2] == '>') {  // for 2>> redirection
      if (strlen(token) != 3) {
        token += 3;
      } else {
        token = strtok(NULL, delimiter);
      }
      pi.stderr = open(token, O_CREAT | O_APPEND | O_WRONLY, 0664);
      if (pi.stderr == -1) {
        pi.errnum = errno;
        fprintf(stderr, "Error opening file: %s, err: %s\n", token,
                strerror(errno));
        return pi;
      }
    } else if (token[0] == '2' && token[1] == '>') {  // for 2> redirection
      if (strlen(token) != 2) {
        token += 2;
      } else {
        token = strtok(NULL, delimiter);
      }
      pi.stderr = open(token, O_CREAT | O_APPEND | O_TRUNC | O_WRONLY, 0664);
      if (pi.stderr == -1) {
        pi.errnum = errno;
        fprintf(stderr, "Error opening file: %s, err: %s\n", token,
                strerror(errno));
        return pi;
      }
    } else if (token[0] == '>') {  // for > redirection
      if (strlen(token) != 1) {
        token += 1;
      } else {
        token = strtok(NULL, delimiter);
      }
      pi.stdout = open(token, O_CREAT | O_APPEND | O_TRUNC | O_WRONLY, 0664);
      if (pi.stdout == -1) {
        pi.errnum = errno;
        fprintf(stderr, "Error opening file: %s, err: %s\n", token,
                strerror(errno));
        return pi;
      }
    } else if (token[0] == '<') {  // for < redirection
      if (strlen(token) != 1) {
        token += 1;
      } else {
        token = strtok(NULL, delimiter);
      }
      pi.stdin = open(token, O_RDONLY, 0664);
      if (pi.stdin == -1) {
        pi.errnum = errno;
        fprintf(stderr, "Error opening file: %s, err: %s\n", token,
                strerror(errno));
        return pi;
      }
    } else {
      char **prevargv = pi.argv;
      pi.argv = malloc((pi.argc + 1) * sizeof(char *));
      for (int i = 0; i < pi.argc; i++) {
        pi.argv[i] = prevargv[i];
      }
      free(prevargv);
      pi.argv[pi.argc] = malloc(strlen(token) + 1);
      strcpy(pi.argv[pi.argc], token);
      pi.argc++;
    }

    token = strtok(NULL, delimiter);
  }
  char **prevargv = pi.argv;
  pi.argv = malloc((pi.argc + 1) * sizeof(char *));
  for (int i = 0; i < pi.argc; i++) {
    pi.argv[i] = prevargv[i];
  }
  free(prevargv);
  pi.argv[pi.argc] = NULL;
  return pi;
}

// Will start the requested program in programInfo and closes the file
// descriptors that have been opened for it Not reentry safe!
struct exitInfo child(struct programInfo *pi) {
  struct exitInfo ei = {0, {}, 0, 0};
  switch (ei.pid = fork()) {
    case -1:
      ei.errnum = errno;
      fprintf(stderr, "Error forking process. err: %s\n", strerror(errno));
    case 0:
      // Set file descriptors
      if (pi->stdin != 0) {
        dup2(pi->stdin, 0);
        close(pi->stdin);
      }
      if (pi->stdout != 1) {
        dup2(pi->stdout, 1);
        close(pi->stdout);
      }
      if (pi->stderr != 2) {
        dup2(pi->stderr, 2);
        close(pi->stderr);
      }
      if (execvp(pi->path, pi->argv) == -1) {
        ei.errnum = errno;
        fprintf(stderr, "Error forking process. err: %s\n", strerror(errno));
      }
    default:
      if (wait4(ei.pid, &ei.wstatus, 0, &ei.usage) == -1) {
        ei.errnum = errno;
        fprintf(stderr, "Error waiting for process. err: %s\n",
                strerror(errno));
      }
      if (pi->stdin != 0) {
        close(pi->stdin);
      }
      if (pi->stdout != 1) {
        close(pi->stdout);
      }
      if (pi->stderr != 2) {
        close(pi->stderr);
      }
      return ei;
  }
}
// -1 return is fatal error or time to exit
// > 0 return is request to exit with that status code
// 0 error is normal
int readCommand(FILE *input) {
  char *line;
  size_t len = 0;
  ssize_t size = getline(&line, &len, input);
  if (size == 0) {
    return -1;
  }
  if (size == 1) {
    return 0;
  }
  if (line[0] == '#') {
    return 0;
  }
  line[size - 1] = '\0';
  struct programInfo pi = tokenizer(line);
  if (pi.errnum != 0) {
    return 0;
  }
  if (strcmp(pi.path, "cd") == 0) {
    if (pi.argc < 2) {
      fprintf(stderr,
              "Error changing directory, directory to switch to must be "
              "specified\n");
    }
    if (chdir(pi.argv[1]) == -1) {
      fprintf(stderr, "Error changing directory, err: %s\n", strerror(errno));
    }
    return 0;
  }
  if (strcmp(pi.path, "pwd") == 0) {
    char buf[PATH_MAX + 1];
    if (getcwd(buf, PATH_MAX + 1) == NULL) {
      fprintf(stderr, "Error getting current directory, err: %s\n",
              strerror(errno));
      return 0;
    }
    fprintf(stdout, "%s\n", buf);
    return 0;
  }
  if (strcmp(pi.path, "exit") == 0) {
    if (pi.argc > 1) {
      errno = 0;
      status = (int)strtol(pi.argv[1], NULL, 10);
      if (errno != 0) {
        fprintf(stderr,
                "Error converting given exit value to integer, err: %s\n",
                strerror(errno));
        return 0;
      }
    }
    exit(status);
  }
  struct exitInfo ei = child(&pi);
  if (ei.errnum != 0) {
    return 0;
  }
  if (WIFSIGNALED(ei.wstatus)) {
    fprintf(stderr, "Child process %i exited with signal %i (%s)\n", ei.pid,
            WTERMSIG(ei.wstatus), strsignal(WTERMSIG(ei.wstatus)));
  } else if (WIFEXITED(ei.wstatus)) {
    if (WEXITSTATUS(ei.wstatus) == 0) {
      fprintf(stderr, "Child process %i exited normally\n", ei.pid);
    } else {
      fprintf(stderr, "Child process %i exited with return value %i\n", ei.pid,
              WEXITSTATUS(ei.wstatus));
    }
  }
  fprintf(stderr, "Real: %.3Lfs, User: %.3Lfs, Sys: %.3Lfs\n",
          (long double)(ei.usage.ru_utime.tv_usec + ei.usage.ru_stime.tv_usec) /
              1000000,
          (long double)ei.usage.ru_utime.tv_usec / 1000000,
          (long double)ei.usage.ru_stime.tv_usec / 1000000);
  status = WEXITSTATUS(ei.wstatus);
  // Error handling
  return 0;
}

int main(int argc, char **argv) {
  FILE *input = stdin;
  if (argc > 1) {
    input = fopen(argv[1], "r");
    if (input == NULL) {
      fprintf(stderr, "Error opening script file: %s, err: %s\n", argv[1],
              strerror(errno));
      return 1;
    }
  }
  int re;
  while ((re = readCommand(input)) == 0)
    ;
  return re;
}
