# Homework #5 File System

**Hand out: 2020-05-01**

**Deadline: 2020-05-08 23:59 (GMT+8) No Extension!**

**TA in charge: SHEN Sijie (ds_ssj \at sjtu.edu.cn )**

Submit: [OC platform](https://oc.sjtu.edu.cn/courses/17894/assignments/37824)

------

1. Symbolic link and hard link are both used as a shortcut of a file but they are very different. Please describe at least 4 differences between them.

2. In some file systems, such as ext4, hard link are not allowed to link a directory. 

	(1) Please give an example to illustrate what problem is if a file system supports hard link to a directory.

	(2) If we really need hard link to a directory, how can you design the file system? Then, please analysis your design from some points like complexity and performance.

3. Please give an example of file system inconsistency caused by system crash. 

4. Explain terms of durability and atomicity, how to enforce durability and atomicity of file system operation?

5. There are three techniques to support consistency: copy-on-write, journaling and log-structured updates. 

	(1) Please explain the differences among them. 

	(2) How to choose these techniques under different scenarios? Please describe these scenarios.

6. For the wear out problem, how can the file system avoid it causing the drop of flash memory’s capacity in a short time of usage? If some block has been worn out, how can the file system handle them? (Suppose the hardware provides some way to report whether a block is worn out.)