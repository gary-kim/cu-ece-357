A) 

B i) This is a directory inode.
    
B ii) There are 5 references to the directory in directory lists. This could be references to the parent directory for directories inside this directory or it could be hard links to this directory (it is impossible to know without more information).

C)

D) There may be another filesystem mounted inside the filesystem the user is attempting to unmount. There may be a program open whose current working directory is inside the filesystem that the user is attempting to unmount. There are file descriptors currently in use by running software that references a file in the filesystem the user is attempting to unmount.

E) The **Inode Cache** helps for identifying each Inode inside the directory. It caches information such as the `nlinks`, the Inode type, permissions, user, and any other information stored on the Inode table about each entry. This helps speed up the `ls -l` operation since `ls - l` lists a lot of information from the Inode table.

TODO: Is any other cache used for this operation? Is there some cache that stores the data (list of names and Inodes) for the directory?


