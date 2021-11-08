#include <stdlib.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <sys/signal.h> 
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <fcntl.h>
#include <alloca.h>

const char* help_message =
    "%1$s - try signals\n"
    "\n"
    "USAGE:\n"
    "\t%1$s signal_number signal_sends\n";

int sig_count = 0;
void signal_handler();
void create_processes(int n, int sig);
int main(int argc, char** argv){
    struct sigaction sa;
    sa.sa_handler=signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    int signal;
    int n;

    if(argc < 3) {
      fprintf(stderr, help_message, argv[0]);
      return 0;
    }

    //get the signal need to be handled from args
    signal = (int)strtol(argv[1], NULL, 10);
    n = (int)strtol(argv[2], NULL, 10);
    if(sigaction(signal, &sa, 0)==-1){
        fprintf(stderr, "error in sigaction: %s\n", strerror(errno));
        return -1;
    }
    create_processes(n, signal);
    printf("Made %i signals received %i signals.\n", n, sig_count);
}

void signal_handler(){
    sig_count++;
    return;
}

void create_processes(int n, int sig){
    pid_t pid = getpid();
    pid_t* children = alloca(n * sizeof(pid_t));
    for(int i=0;i<n;i++){
        int child = fork();
        switch(child){
            case -1:
                fprintf(stderr, "error forking process: %s\n", strerror(errno));
                exit(1);
                break;
            case 0: 
                if(kill(pid, sig)==-1){
                    exit(errno);
                }
                exit(0);
                break;
            default:
                children[i] = child;
                break;
        }
    }
    for(int i=0;i<n;i++){
        int wstatus;
        if (waitpid(children[i],&wstatus,0) != children[i]) {
          fprintf(stderr, "error: unexpected return waiting for child process: %s", strerror(wstatus));
        }
        if (WIFSIGNALED(wstatus)) {
          fprintf(stderr, "error: a child process has been killed by signal %i (%s)", WTERMSIG(wstatus), strsignal(wstatus));
          exit(1);
        }
        if (WEXITSTATUS(wstatus) != 0) {
          fprintf(stderr, "error from a child process calling \"kill\": %s\n", strerror(WEXITSTATUS(wstatus)));
          exit(1);
        }
    }
}