a)
    `SIGINT` is delivered (Sig #2 on Linux).

b)
    `SIGSEGV` is delivered (Sig #11 on Linux).

c)
    From the man page (man 7 signal)::
        The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored.
    So the answer is `SIGKILL` (Sig #9 on Linux) and `SIGSTOP` (Sig #19 on Linux).

d)
    'SIGCONT' is delivered (Sig #18 on Linux).
    The signal `SIGCONT` will be dilivered only when the process is stopped in the background. In this case, the background process is stopped because either the process is trying to read from terminal in the background, in which case a `SIGTTIN` is sent to stop the process, or trying to print something from background to the terminal that the terminal configuration does not allow, for example, terminal setting changes, in which case a `SIGTTOU` is sent to stop the process. The terminal may also be set to not allow any output to it from background processes by setting the `tostop` settings of terminal.

