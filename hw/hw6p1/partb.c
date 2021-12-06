struct ll {
    struct ll *fwd;
    /* and other stuff  */
}

void ll_insert(struct ll *where, struct ll *what) {
    sigset_t nmask, omask;
    sigset_fill(&nmask);
    sigprocmask(SIG_BLOCK, &nmask, &omask);

    what->fwd = where->fwd;
    where->fwd = what;

    sigprocmask(SIG_SETMASK, &omask, NULL);
}
