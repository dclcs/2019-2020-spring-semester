# ChCore Lab 5: File system and Shell

**Hand out: 2020-05-01**

**Deadline: 2020-06-10 23:59 (GMT+8) No Extension!**

**TA in charge: SHEN Sijie (ds_ssj \at sjtu.edu.cn )**

## Introduction

In this lab, you will implement `tmpfs`, an inode-based in-memory file system, and a user-level file system server. Then, you will implement enough libraries to run a shell on the console. These features need a file system, and this lab introduces a simple read/write file system. The file system you will implement in ChCore is a small but powerful enough to provide the basic features of a file system, including creating, reading, writing, and deleting files. 

This lab is divided into two parts: In **Part A**, you will implement a file system server based on `tmpfs`. Since ChCore is a micro-kernel, the file system is supported by a user-level file system server process. In this part, you need to implement some basic functions of this file system server. In **Part B**, you will implement a user-level shell to interact with users, such as the one you have used in UNIX. You need to implement some basic commands in this shell, and you can also design and implement any functions you would like to.

### Getting Started

As we have done in previous labs, you need to complete your Lab 4 and commit all updates on lab 4. Then, fetch the latest remote lab repo and build branch `lab5` based on `upstream/lab5` . You will have an empty `lab5`, so you need to merge your solutions for Lab 1 to Lab 4 to the branch `lab5`. After resolving some conflicts, you can commit this branch and push to your remote repo. We also give you specific `git` commands like the following:

```
# commit all your previous solution of lab4
$ git commit -am 'my solution to lab4'

# fetch the remote updates, you are in branch lab4
$ git fetch upstream 
    
# switch to the local branch lab5 
# this branch based on empty lab5 code of the remote repo
$ git checkout -b lab5 upstream/lab5 
Branch 'lab5' set up to track remote branch 'lab5' from 'upstream'.
Switched to a new branch 'lab5'

# merge solution for lab1~4  to lab5
$ git merge lab4
Merge made by recursive.

# commit the megre to branch lab5
$ git commit -am "merged previous 4 lab solutions"

# update the remote tracking branch to your origin repo 
# instead of the upstream repo
$ git push -u origin    
To https://ipads.se.sjtu.edu.cn:2020/[username]/chcore.git
 * [new branch]      lab5 -> lab5
Branch 'lab5' set up to track remote branch 'lab5' from 'origin'.
```

### Grading and Hand-In Procedure

**Grading**: coding (80%) + document (20%)

You need to finish all the exercises and questions stated in the lab document.

To check your code grade, type:

```
chcore$ make grade
xxx
xxx
Score: 80/80
```

You should hand-in a document to report how you do the exercises and answer the questions. In Lab 5, the document is named as `[StudentId]-lab5.pdf` (`.md, .txt`) and stores under the `doc` directory (`mkdir doc` to create it). You can add it to your repo as:

```
chcore$ git add doc/[StudentId]-lab5.pdf
chcore$ git commit -m "add lab5 document"
chcore$ git push origin lab5
```

When you finish Lab 5, please submit it with:

```
chcore$ git add .
chcore$ git commit -m"finish lab5"
chcore$ git push origin lab5
```

**Note**: 

1. Each `git push`, make sure your lab can be built successfully; otherwise, you will get 0 points.
	1. **In this lab, we do not restrict the file you modify or add, however, do not modify files or scripts for test**.
	2. Except for the local test set, we also prepare a remote test set to test your code. The eventual score of your code is combined with both the local test set and the remote test set.

**Please directly contact to the TA, if you:**

1. find bugs or mistakes in this lab, or
2. have suggestions.

## Part A: File system

### In-memory File System

`tmpfs` is an inode-based in-memory file system. Like other inode-based file systems, it divides available (in-memory) disk space into two main types of regions: *inode* regions and *data* regions. Each file has an inode, which holds some meta-data about the file, such as file size, and pointers to data blocks of this file. The data regions stores file data and directory meta-data. 

In the in-memory file systems, regular files logically consist of a series of memory pages, which may be scattered throughout the memory, like the continuous pages in virtual memory can be scattered throughout physical memory. Directories stores a hash table mapping from filename to directory entry `dentry`. The file system environment handles all modifications to directories internally as a part of performing actions such as file creation and deletion. Users can read structure of directory directly (by `tfs_scan`), which means that user environments can perform directory scanning operations themselves (e.g., to implement the `ls` command). The disadvantage of this approach to directory scanning, and the reason most modern UNIX variants discourage it, is that it makes application programs dependent on the format of directory meta-data, making it difficult to change the file system's internal layout without changing or at least recompiling application programs as well.

### FS Structure in ChCore

The `tmpfs` in ChCore is defined and implemented in `user/tmpfs/tmpfs.{h,c}`, as the following figure.

![tmpfs](../../../../../../.././image/tmpfs.png)

**Inode**. The structure `inode`, it records its type (regular/directory) and its data size. The interface `new_dir` and `new_reg` are used to create these two types of inodes.

**Directory**. In directory inode, it stores a pointer to a hash table `htable` mapping from hash values of file names to there directory entry `dentry`. This hash table consists of buckets, and each bucket stores a linked list of `dentry`. `hlist_node`is used to link `dentry`s. `tmpfs_root` is a special inode, similar to the *super block*in disk file systems. This root the first inode to search a file by its name.

**Regular file**. In regular file inode, it stores a root to the data. Data are orginized as a radix tree, and the leaf nodes of this tree are memory pages, which size is `PAGE_SIZE`. This tree is searched by page number (offset_in_file / `PAGE_SIZE`).

### File operations

We have provided a variety of functions in `user/tmpfs/tmpfs.c` to implement the basic facilities you will need to interpret and manage `tmpfs` structures, scan and manage the entries of directory-files, and walk the file system from the root to resolve an absolute pathname. Read through *all* of the code in `user/tmpfs/tmpfs.c` and make sure you understand what each function does before proceeding.

> **Exercise 1.** Implement `tfs_mknode` and `tfs_namex` in `user/tmpfs/tmpfs.c`. `tfs_mknode` creates a directory inode or a regular file inode under a parent. This function can be used to create files or directories. `tfs_namex` walk the file system structure to locate a file. Before your implementation, please understand the functions defined in `tmpfs.h` carefully. They are very useful.

Read and write are basic functions of a file system. In `tmpfs`, read or write is only read/write data in memory into/from a memory buffer.

> **Exercise 2.** Implement `tfs_file_read` and `tfs_file_write` in `user/tmpfs/tmpfs.c` to support read and write. They read/write a file at an offset (similar to `seek` in a disk fs) for a length. The size of a data page is `PAGE_SIZE`, so the operations may affect several pages or create pages in write. Remember, the read cannot exceed file size and write may increase the file size. The return value of these two functions is the real size of read or write operations.

In Chcore Lab 3, we tell you that you can use `make user` to build user mode applications as ELF binary files (`.bin` or `.srv` under `ramdisk`) and we use `cpio`to concatenate these files into a single file called `cpio` archive. Each member file contains a header (as described in [this reference page](https://www.mkssoftware.com/docs/man4/cpio.4.asp) and defined in `user/lib/cpio.h` in ChCore) followed by file contents. The end of the archive is indicated by another header describing an (empty) file named `TRAILER!!`. Before we have a file system, we naively copy the content of `ramdisk.cpio` into a continuous memory without file structures. To manage the content of `ramdisk.cpio` in our `tmpfs`, you need to load the content of this archive by `tfs_load_image`.

> **Exercise 3.** Implement `tfs_load_image` in `user/tmpfs/tmpfs.c` to support tmpfs can load the content from a `cpio` archive. You need to create directories and files if necessary, and write the data into the `tmpfs` by functions you implemented before.

### FS server

Now that we have the necessary functionality within the file system environment itself, we must make it accessible to other environments that wish to use the file system. Since other environments can't directly call functions in the file system environment, we'll expose access to the file system environment via a *remote procedure call*, or RPC, abstraction, built atop ChCore's IPC mechanism. Graphically, here's what a call to the file system server (say, read) looks like the following figure.

```
      Regular env           FS env
   +---------------+   +---------------+
   |    fs_read    |   |  tfs_file_read  |
   |     (app)     |   |    (tmpfs.c)    |
...|.......|.......|...|........^........|...............
   |       |       |   |        |        | RPC mechanism
   |       |       |   |  fs_server_read |
   |       |       |   | (tmpfs_server.c)|
   |       |       |   |        ^        |
   |       |       |   |        |        |
   |       |       |   |fs_server_dispatch|
   |       |       |   |  (tmpfs_main.c) |
   |       |       |   |        ^        |
   |       v       |   |        |        |
   |   ipc_call    |   |    ipc_recv     |
   |       |       |   |        ^        |
   +-------|-------+   +--------|--------+
           |                    |
           +--------------------+
```

Everything below the dotted line is simply the mechanics of getting a read request from the regular environment to the file system environment. Starting at the beginning, `fs_read` (implemented by applications) works on any file name. The client uses `ipc_call` to send messages to the file systems server.

The file system server code can be found in `user/tmpfs/tmpfs_main.c`. It loops in the `main` function, endlessly receiving a request over IPC, dispatching that request to the appropriate handler function by `fs_server_dispatch`, and sending the result back via IPC. In the read example, `fs_server_dispatch` will dispatch to `fs_server_read`, which is defined in `user/tmpfs/tmpfs_server.{h,c}` and takes care of the IPC details specific to read requests such as unpacking the request structure and finally call `tfs_file_read` to actually perform the file read.

> **Exercise 4**. Here is a table to help you understand the dispatch rules of `fs_server_dispatch`. Implement `fs_server_dispatch` defined in `user/tmpfs/tmpfs_main.c` and all of `fs_server_xxx` functions defined in `user/tmpfs/tmpfs_server.c` as the following table to deal with different ipc requests. Remember how ChCore deal with shared memory pages.
>
> | Req ID          | Function             | Meaning                     |
> | --------------- | -------------------- | --------------------------- |
> | FS_REQ_SCAN     | `fs_server_scan`     | Scan all files under a path |
> | FS_REQ_MKDIR    | `fs_server_mkdir`    | Make a directory            |
> | FS_REQ_CREAT    | `fs_server_creat`    | Create a regular file       |
> | FS_REQ_RMDIR    | `fs_server_rmdir`    | Remove a directory          |
> | FS_REQ_UNLINK   | `fs_server_unlink`   | Remove a regular file       |
> | FS_REQ_WRITE    | `fs_server_write`    | Write a file                |
> | FS_REQ_READ     | `fs_server_read`     | Read a file                 |
> | FS_REQ_GET_SIZE | `fs_server_get_size` | Get the file size           |

After completing Exercise 4 correctly, you can get 30 points by typing `make grade`. 

```
LAB5: test >>>>>>>>>>>>>>>>>
./scripts/grade-lab5
runing fs_test: (9.4s)
  fs server init: OK
  fs scan: OK
  fs write read: OK
  fs deepfile: OK
  ……
Score: 30/80
```

## Part B: The Shell

The last mission in the ChCore lab is to implement a user-level shell, which is define in `user/apps/init_main.c`. It has two main components: the input parser and the client of `tmpfs`. In this part, we only give you the skelaton in `user/apps/init_main.c` and `user/apps/init.c`. Please implement some functions as we used in UNIX shell. 

You can type the following commands to run your shell:

```
chcore$ make user
……
copy user/*.bin to ramdisk.
copy user/*.srv to ramdisk.
add fs_test files
1944 blocks
succeed in building ramdisk.
chcore$ make build bin=init
……
before ninja
[63/63] Linking C executable kernel.img
after ninja
chcore$ make qemu-nox
……
init loads cpio image.
Booting fs...
info_page_addr: 0x200000
[tmpfs] register server value = 0
fs is UP.

$
```

The `$` is a prompt, and you can type a command after the prompt. However, since you have not implemented `readline()` defined in `user/apps/init.c`, you cannot see what you have typed.

> **Exercise 5**. Implement `getchar()` defined in `user/apps/init_main.c` to get a character from standard input each time, and `readline()` defined in `user/apps/init.c`. This function can store what you have typed before pressing enter in a memory buffer and put it on `stdio`. You can use I/O functions defined in `user/lib/syscall.{h,c}`.

> **Exercise 6**. Implement `bultin_cmd()` defined in `user/apps/init.c` to support built-in commands in shell, e.g., `ls`, `ls [directory]`, `cd [directory]`, `echo [string]`, `cat [filename]`. 

> **Exercise 7**. Implement the function `readelf_from_fs` defined in `user/lib/liblauncher.c` and `run_cmd` defined in `user/apps/init.c` to support running a executable file by type it name. You should also implement the auto-complement by `tab` (**following the order of the files in the cpio archive**). We add function declaration of `launch_process_with_pmos_caps` in `user/lib/proc.h`

> **Exercise 8**. Implement `top` to show information of threads. The format of top is like the following: (you can also add another information on the tail of each line):
>
> ```
> $ top
> Current CPU 0
> ===== CPU 0 =====
> Thread ffffff0020600228 Type: ROOT      State: TS_RUNNING       CPU 0   AFF 0   Budget 2
> Thread ffffff00206007a8 Type: USER      State: TS_READY         CPU 0   AFF 0   Budget 2
> ===== CPU 1 =====
> Thread ffffff00001ecfa8 Type: IDLE      State: TS_RUNNING       CPU 1   AFF 1   Budget 2
> ===== CPU 2 =====
> Thread ffffff00001ed000 Type: IDLE      State: TS_RUNNING       CPU 2   AFF 2   Budget 2
> ===== CPU 3 =====
> Thread ffffff00001ed058 Type: IDLE      State: TS_RUNNING       CPU 3   AFF 3   Budget 2
> ```

After finishing Exercies 8, you will get all of the 80 points.

```
LAB5: test >>>>>>>>>>>>>>>>>
./scripts/grade-lab5
runing fs_test: (9.1s)
  fs server init: OK
  fs scan: OK
  fs write read: OK
  fs deepfile: OK
runing init_test: (9.3s)
  readline: OK
  echo: OK
  ls: OK
  ls dicrectory: OK
  cat: OK
  auto complement: OK
  run executable: OK
  top: OK
```

 