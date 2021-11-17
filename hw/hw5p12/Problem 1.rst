a)
    See attached sketch.
b)
    6 page frames allocated
c)
    The BSS region must be a minimum of 5 pages big. The beginning address for the BSS region is `08511000` and the ending address (that we know of) is `08516FFF`. There are 5 pages worth of space between those virtual addresses meaning the BSS region must be at least 5 pages big.
d)
    Yes, the PFRA has reclaimed the page frame pointed to by the virtual address `08516000`. This is evident by the fact that the PTE for that address has the present bit set to `0` but it still has other bits set which should be pointing to where in which swap space that page frame can be found in again.
