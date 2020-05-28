# SE-315

## Lab 5 Report

### Part A: File System

#### Exercise 1

This exercise is fairly easy. But we should keep these facts in mind:

* Each object (file or directory) is arranged by a unique `inode`.
* Each directory has a field `dentries` to index its direct child files & directories.
* Peer  `dentries` are connected via `hlist_head`.
* Each object (`inode`) will also appear as a `dentry` of its parent directory's `dentries`, except the root `inode`.

#### Exercise 2

Now we're going to implement `file_read` and `file_write`.

Similar functions have been implemented in the Lab 1 of `SE-227` (CSE). But here we don't have to manipulate those blocks, since a block managing system based on radix tree has been implemented for us.

We just need to make sure using a corresponding way to calculate radix tree's key in `file_read` and `file_write`, which is much neater than the `CSE` file system.

