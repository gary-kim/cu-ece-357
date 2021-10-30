a)
    `SIGINT` is delivered (Sig #2 on Linux).

b)
    `SIGSEGV` is delivered (Sig #11 on Linux).

c)
    From the man page (man 7 signal)::
        The signals SIGKILL and SIGSTOP cannot be caught, blocked, or ignored.
    So the answer is `SIGKILL` (Sig #9 on Linux) and `SIGSTOP` (Sig #19 on Linux).

d)
    TODO: Need to figure out

