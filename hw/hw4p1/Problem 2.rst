a)
    Running the command `kill -n 10 <PID>` will send a `SIGUSR1` signal to the process.

b)  
    `sigprocmask` will not mask the `SIGUSR1` signal while the `handler` function is being run. This means that if another `SIGUSR1` signal comes in while inside the `handler` function, the `handler` function will be run on top of the `handler` function. The reason for this is because the `SA_NOMASK` flag is set.

    One issue though, is the above is only true if `sa_mask` does not mask the `SIGUSR1` signal (according to `man 2 sigaction`). Since `sa.sa_mask` is not explicitly set and it is present on the stack, its contents are not guaranteed so it may have accidentally masked `SIGUSR1`. In that case, `SA_NOMASK` becomes ineffective and the `SIGUSR1` signal will be masked while the handler is running.

    We would suggest running `sigemptyset(&sa.sa_mask)` before running `sigaction`. Otherwise, it is possible that random signals will be masked unintentionally since the `sigaction` struct was defined as a local variable meaning it is on the stack meaning there is no guarantee about what it will be set to when first declared.