# Assignment 2

## Question 1

### Q

Assume you are calling `fork` in one thread of a multi-thread process in Linux. How many threads will there be in the child process? What problem may exist in the `fork`ed child process? Why Linux have to design fork operation of a multi-thread process like that?

### A

Only the active thread (the one who invokes that `fork` call) will be copied into the `fork`ed process. Specially, their thread ID will be identical, too.

That means except that special invoker thread, all other threads will simply disappear in the `fork`ed process, which is weird and unpromising.

I assume the reason of `fork`'s bad performance with multi-thread scenario is when `fork` was initially created in POSIX, there's no actual multi-thread developing model yet.

## Question 2

### Q

`vfork` is a system call similar to `fork`. Use `man vfork` to learn more about `vfork`. Why does `vfork` perform better than `fork` in applications consuming a huge amount of memory even when `fork` already applied the COW policy for memory copying? Try to optimize the performance of `fork` for such applications without compromising its semantic refers to the idea of `vfork`.

### A

`man vfork` declares that `vfork` spawns new process in a virtual memory efficient way.

Its such advantage was brought by this strategy: When `vfork` creates a new process, it  doesn't fully copy the address space of the old process. Subprocesses created by invoking `vfork` simply borrows its parent's memory and thread of control, while `fork`ed ones (with CoW) still needs to create its specific virtual memory address.

If we want to optimize `fork`'s implementation, we might avoid creating all virtual memory space once the subprocess os created, but omit them until one address is actually modified by the subprocess.

## Question 3

### Q

In an operating system where a single process contains multiple threads like ChCore, a context switch is usually done at the granularity of thread. However, context switch between threads belonging to the same process can be faster than that between threads belonging to different processes. Why?

### A

Switching threads belonging to different processes: needs to refresh whole memory space and registers, since they're not placed in the same process context.

Switching threads belonging to the same process: simply switch the registers, and re-filling stack pointer registers to switch its stack. It is much lower a cost.

## Question 4

### Q

In ChCore, the page table mapping has been changed during the context switch. However, after the page table changed, the ChCore kernel, which also uses virtual memory address in its code, can still run correctly and get the correct address of its variables and codes. Why?

### A

Because in ChCore OS, the higher-end memory address is always reserved by OS kernel, no matter in which process context, which is mapped to the same kernel physical memory space. So kernel can execute correctly in every user application context.

## Question 5

### Q

For user-level threads like coroutine, since all coroutines belong to the same thread, only one coroutine can be running at the same time. How can coroutine benefit the performance with the same amount of computing resources? What kind of applications can benefit from coroutine most?

### A

Actually, threads is already very tiny. However, it still needs to trap into the kernel to switch between threads, which is a great cost if the switching frequency is rather high.

If we use user-level threads, there's no need to trap into kernel to switch between threads. We just need to use branch instruction to alter the control flow.

Those application requires frequently switched threads will benefit a lot by using techniques like co-routine.

## Question 6

### Q

Now, Assume you are going to create an operating system specialized for the scenario where only one single web server application is running in the OS. The web server requires: 1) Low waiting and processing latency for every user request. 2) Code handling requests from different users can not access each other's memory. How are you going to design the semantic of process/thread in your OS and the program logic of the web server? What should be stored in PCB/TCB, and how is context switch done in your OS?

### A

I think it's appropriate to use one single process as the main web server application, and use multiple co-routines to implement multi-processing.

First, since this OS is a highly embedded and specific system, we should limit that there's only one foreground process, the web server application. That should optimize the resource usage percentage.

Second, we should ensure that every single request should be handled asap. So the context switch must happen frequently. Using co-routines can minimize the context switching cost.

In the specific OS, context switch rarely happens. Most CPU time is scheduled to the main web server process, and sometimes it will execute some background routine jobs, such as backup, analysing, etc.

However in the web server application, co-routine context switch happens more frequently. Since user-model context switching doesn't cost much, we can make it happen more frequently.

PCB Fields:

* Process ID
* Memory Space
* Registers
* Priority
* Status
* ...

TCB Fields:

* Thread ID
* Registers
* *Web Request ID
* *Web Request Type
* Priority
* Status
* ...

