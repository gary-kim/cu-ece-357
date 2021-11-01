a)
    Running the command `kill -n 10 <PID>` will send a `SIGUSR1` signal to the process.

b)
    TODO: Expand on this answer. Most importantly, `sigemptyset` should probably be set here.
    
    `sigprocmask` will not mask the `SIGUSR1` signal while the `handler` function is being run. This means that if another `SIGUSR1` signal comes in while inside the `handler` function, the `handler` function will be run on top of the `handler` function. The reason for this is because the `SA_NOMASK` flag is set.

    We would suggest running `sigemptyset(&sa.sa_mask)` before running `sigaction`. Otherwise, it is possible that random signals will be masked unintentionally since the `sigaction` struct was defined as a local variable meaning it is on the stack meaning there is no guarantee about what it will be set to when first declared.