struct ll {
    struct ll *fwd;
    char spinlock;
    /* and other stuff  */
}

void ll_insert(struct ll *where, struct ll *what) {
    while (TAS(&where->spinlock) != 0);

    what->fwd = where->fwd;
    where->fwd = what;

    where->spinlock = 0;
}
