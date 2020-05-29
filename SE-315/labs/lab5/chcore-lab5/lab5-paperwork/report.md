# SE-315

## Lab 5 Report

### Part A: File System

#### Exercise 1

This function isn't hard to implement. But we should keep these facts in mind:

* Each object (file or directory) is arranged by a unique `inode`.
* Each directory has a field `dentries` to index its direct child files & directories.
* Peer  `dentries` are connected via `hlist_head`.
* Each object (`inode`) will also appear as a `dentry` of its parent directory's `dentries`, except the root `inode`.

#### Exercise 2

Now we're going to implement `file_read` and `file_write`.

Similar functions have been implemented in the Lab 1 of `SE-227` (CSE). But here we don't have to manipulate those blocks, since a block managing system based on radix tree has been implemented for us.

We just need to make sure using a corresponding way to calculate radix tree's key in `file_read` and `file_write`, which is much neater than the `CSE` file system.

#### Exercise 3

In this part, we can make use of our self-implemented functions in Exercise 1 and Exercise 2.

First, we should call `tfs_namex` to determine the parent folder of our target file (we should create those intermediate folders if they don't exist) and the actual file name without absolute path prefix. Then, we have the pointer that points to its parent `inode`.

Now, with its name and parent `inode`, we can call our `tfs_mknode` to create this file.

> Actually, the function gets called here is `tfs_creat`, which is merely a thin wrapper of `tfs_mknode`.

Now, our `tmpfs` can already pass `fs_server_init` and `fs_scan` in `make grade`, though these requests haven't been sent to `tmpfs`. This just indicates that `tfs_load_image` function is working basically normally without crash in the initialization phase.

#### Exercise 4

Currently, we can't pass `fs_read_write` since these requests haven't been properly handled in our `tmpfs_server`. Now let's fix this problem.

First, we can make function calls in our `fs_dispatch` handler defined in `tmpfs_main.c`. All parameters can be found in `fs_request` data structure, so it's simple to fill in them all.

> One weird thing I can't figure out is I can't find a field for `scan`'s `start` parameter. And the `flags` field in `fs_request` isn't used, either.

For those `fs_server` functions, things are much easier. They're basically a wrapper function of those defined in `tmpfs.c`. No much coding is needed here.

> Notice that in the testing script, it didn't use any kind of IPC function calls. That means we could possibly pass `fs_test` even if we didn't implement `fs_dispatch`. Weird.

### Part B: The Shell

Finally! Our baby OS will be able to hear and speak soon!

#### Exercise 5

This `readline` function is much simpler than the GNU `readline` library. We just need to handle input char by `usys_getc`. 

If it's a normal character, put it in buffer and echo it. 

if it's a backspace (`\b`) and the buffer isn't empty, pop out the last input char.

If it's a newline (`\n`), we should break the loop.

#### Exercise 6

