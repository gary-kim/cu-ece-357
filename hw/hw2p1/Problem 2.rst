a)
    This helps to keep information that would often be accessed right after one another closer together. For instance, the free map for a specific part of the disk is close to that part meaning the hard drive head does not need to move as far. All in all, this system helps to prevent the drive read/write head having to move as far.

    This isn't particularly useful for SSDs as they have no penalty for random access. Hard drives need to move the read/write head over to where information is, slowing down non-sequential data access but SSDs have no moving parts so sequential access is just as fast as random access.

b)
    i. This is a directory inode.
    
    ii. There are 5 references to the directory in directory lists. This could be references to the parent directory for directories inside this directory or it could be hard links to this directory (it is impossible to know without more information).

c)
    There is space taken up by non-data-blocks. Things like the Inode table, header, and free map take up extra space on the drive which can no longer be taken by the data itself.

    The system may also have been set up with a reserve factor. This means that part of the drive is reserved and only usable by the `root` user in order to help avoid fragmentation on the drive.

d)
    1. There may be another filesystem mounted inside the filesystem the user is attempting to unmount. 
    2. There may be a program open whose current working directory is inside the filesystem that the user is attempting to unmount.
    3. There may be file descriptors currently in use by running software that references a file in the filesystem the user is attempting to unmount.

e) 
    The **Inode Cache** helps for identifying each Inode inside the directory. It caches information such as the `nlinks`, the Inode type, permissions, user, and any other information stored on the Inode table about each entry. This helps speed up the `ls -l` operation since `ls - l` lists a lot of information from the Inode table.

    TODO: Is any other cache used for this operation? Is there some cache that stores the data (list of names and Inodes) for the directory?


