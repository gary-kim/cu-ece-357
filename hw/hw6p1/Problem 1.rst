A)
    It is possible that while one thread executes the first line and sets `what->fwd = where->fwd`, while another thread sets `what2->fwd = where->fwd`, then the first thread sets `where->fwd = what`, then the 2nd sets `where->fwd = what2`. In this case, the linked list will end up looking like the following:

    where -> what2 -> next
             what -----^

    Essentially, `what` got completely lost in the list and is no longer a part of the list. So from the perspective of the functions running the `ll_insert` function, one of the inserts simply did not work (i.e. did not insert the `what`).

B)
