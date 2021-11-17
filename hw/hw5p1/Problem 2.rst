a)
    The page fault resolution algorithm determines that it is close to a `GROWSDOWN` area and expands the stack region to cover the faulted address. This is a minor page fault.

b)
    The page fault resolution algorithm will allocate a new page frame then load that part of the file into that page frame then set up the page table entry to point to that new page frame. This is a major page fault.

c)
    The page fault resolution algorithm will allocate a new page frame then attempt to load that part of the file into the page frame. Unfortunately, since the file no longer exists, an I/O error will occur so the kernel will send a `SIGBUS` to the process.
d)
    The virtual memory space the process is attempting to read does not have execute permissions. Therefore, a `SIGSEGV` is sent to the process.

e)
    A page frame will be allocated then a one time inode associated with `/dev/zero` (which is all zeros) will be created. The contents of this one time inode's file will be loaded into that page frame, then the page table entry set up to point to the new page frame. This is likely a minor fault as while it is loading in from `/dev/zero`, `/dev/zero` is a character device so I/O is never actually reached.

2f)
    When the other process attempts to write to the file, the kernel will check if the file is already in memory by looking at the struct inode for the file which has a pointer to the struct address_space of the file which has a pointer to the radix-64 tree that maps each offset of the file to struct page in the mem_map array. At this point, the write syscall will find that the offset of the file it is attempting to write to is already mapped in memory since it is defined in the radix-64 tree, and simply write the contents into that location in memory instead.

    One of the effects of this is that since it was written into the same page frame as that same offset in `p` of the original program, the change is instantly visible in the original program's `p` array as well.
