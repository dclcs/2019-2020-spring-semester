# Homework 5

## Question 1

### Q

Symbolic link and hard link are both used as a shortcut of a file but they are very different. Please describe at least 4 differences between them.

### A

1. Soft link (or symbolic link) relies on specific full path of its origin, because it only contains the path of it. So if the origin file is moved or renamed, the symbolic link will be invalid. However, hard linking won't be affected by that.
2. If we remove the origin file, all its soft links will be invalid. However, if it has at least one hard link, then they will still be fine and its content blocks will not be recycled.
3. Generally you can't hard-link a directory (since they are not files, actually), but you can always soft-link one.
4. Hard linking can't be made across file systems, since it relies on the file system's `ref_cnt`. Soft linking is fine, anyway. You can even soft-link a virtual file. (like a socket, an I/O device, etc.)

## Question 2

### Q

In some file systems, such as `ext4`, hard link are not allowed to link a directory. 

1. Please give an example to illustrate what problem is if a file system supports hard link to a directory.

2. If we really need hard link to a directory, how can you design the file system? Then, please analysis your design from some points like complexity and performance.

### A

#### 1

In most file system's implementation, directory merely contains its children nodes' name and full path.

So we may think directory as a more composite, complicated soft link. Just hard-linking its content is totally pointless. So if we want to keep the syntax of "hard link", we should recursively hard-link its children nodes, which may contain both files and directories.

Notice that this may cause dead-loop if linking path contains itself.

#### 2

Really?

* We should check if the target linking path is a part of the origin path. If so, reject the linking procedure.
* Don't recursively hard-linking all its children at once. Only when a "breaking" operation happens should we actually make a hard-linking. Similar to CoW's idea, we only need to hard-linking those files when they're modified, renamed, deleted, etc.

## Question 3

### Q

Please give an example of file system inconsistency caused by system crash.

### A

Let's take creating new files as an example.

* Firstly, obtain a new unallocated `inode` number;

* Secondly, obtain several unallocated blocks and write data into it;

* Thirdly, write `inode` and `block` bitmap in synchronize, and update super block contents.

If step 1) and 2) are done successfully, but the system crashes down before 3) could be done. Now there's an inconsistency situation between metadata field and data field.

## Question 4

### Q

Explain terms of durability and atomicity, how to enforce durability and atomicity of file system operation?

### A

Term Explanation.

* **Atomicity**: all operations in one single transaction should be either *all of them done* or *none of them done*.
* **Durability**: once a transaction is committed, it should confirm its data permanently solidified in the database.

How to make sure that?

* We can use **transaction** to ensure its atomicity.
	* If an error occurs halfway through a sequence of writes, the transaction should be **aborted**, and the writes made up to that point should be **discarded**.
	* In other words, the database saves you from having to worry about **partial failure**, by giving an all-or-nothing guarantee.
* It's more complicated to ensure its durability.
	* You can announce *committed* with confidence only after your data written to non-volatile storages, like disk, tape, etc.
	* And, for a cluster system, we should make sure the updated data copied into enough nodes that could trigger a correct consensus.

## Question 5

### Q

There are three techniques to support consistency: copy-on-write, journaling and log-structured updates.

1. Please explain the differences among them. 

2. How to choose these techniques under different scenarios? Please describe these scenarios.

### A

#### Copy on Write

When we need to update a file, we find a new segment to store the difference, and keep the origin file untouched.

The problem is one file could contain data from everywhere, causing great loss when accessing its content.

This technique might be helpful when it holds lots of `git` version control systems.

#### Journaling

Every time you want to perform a breaking action, write it in journal first. After everything's ready, make a commit to solidify data.

The main problem is obvious: too slow. We must write data twice: writing into journal and committing a transaction.

The advantage is if you choose the full journaling mode, you can redo them all after the crash without any data loss.

#### Log-structured Updates

That's a tricky solution. It requires all files written into disk sequentially. Any update will be written in the tail of writing pointer. That means before the disk is filled up, we will never overwrite any data.

This could also make a full recovery from the crash: all history version of files are kept well, just stored in earlier segments.

The disadvantage is it takes too much disk space, and requires frequent garbage collection. Also, for a HDD, frequently accessing the earlier segment might cause its life loss.

## Question 6

### Q

For the wear out problem, how can the file system avoid it causing the drop of flash memory’s capacity in a short time of usage? If some block has been worn out, how can the file system handle them? (Suppose the hardware provides some way to report whether a block is worn out.)

### A

Maybe our file system can provide an extra layer to map the virtual disk address and physical disk address.

In such mapping we can block those worn-out blocks to keep the availability of the whole disk.

Moreover, we can also dynamically adjusts the physical disk block usage frequency to maximize the FLASH drive's lifetime.

If only that won't cause great performance loss.