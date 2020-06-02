# Homework 6

## Question 1

### Question

What are the main abstractions in Linux device driver model? Please navigate the `sysfs` ﬁlesystem and ﬁnd out their relationships.

### Answer

#### Abstractions

The main abstraction of LDDM is a general description of system architectures.

A general LDDM object should handle these tasks:

* Power Managing & System Poweroff
* Communicate with user space
* Handle Hot Plug
* Device Category
* Manage Object Life Cycle

#### `sysfs`

As a virtual memory file system, it is always mounted to `/sys`. 





## Linux Modules

```
 |------------> M M <------------|
 |               ^               |
 |               |               |
 v               v               |
VFS ---------> SCHED <--------- IPC
 |
 |
 |----------->  NET
```

* `SCHED` evenly manages CPU resources
* `MM` ensures each process' memory managing safety
* `VFS` is the illuson of all extern devices
	* including disks, FLASH drives, I/O devices and display devices
* `NET` manages all network stuff
* `IPC` manages inter-process communication stuff

