# ChCore Lab 4: Multiprocessing

**Hand out: 2020-04-17**

**Deadline: 2020-05-27 (GMT+8) No Extension!**

**TA in charge: Dong Zhiyuan (zydong829 \at sjtu.edu.cn )**

## Introduction

In this lab, you will support preemptive multitasking among concurrent threads running on multiple processors, creating a new process executing given binary file, communication between different processes. This lab consists of four parts.

In part A, you will first boot more than one CPUs to support multiprocessing, add the big kernel lock to solve concurrency problems.

In part B, you will implement a basic round-robin scheduler to support basic thread scheduling among all the CPUs. You will also support the hardware timer interrupts and preemption.

In part C, you will implement a user-mode function called `spawn()`, which creates a new process to run a specific program.

In part D, you will add support for inter-process communication (IPC), allowing different user-mode processes to interact with each other.

### Getting started

At this point, you should have finished your Lab 3. Use Git to commit your Lab 3 solutions. Then you should fetch the latest version of the lab repository, create a local branch called lab4 based on TA's lab4 branch, and merge your solutions to that branch. Finally, commit all the changes to branch origin/lab4, and you can start your Lab4:

```
# commit all your previous solution of lab3
$ git commit -am 'my solution to lab3'

# fetch the remote updates, you are in branch lab3
$ git fetch upstream 
    
# you switch to the branch lab4, whose code is based on the 
# empty code provided by the TAs, you are in branch 4
$ git checkout -b lab4 upstream/lab4 
Branch 'lab4' set up to track remote branch 'lab4' from 'upstream'.
Switched to a new branch 'lab4'

# you need to merge your lab1~3's solutions to lab4 
$ git merge lab3
Merge made by recursive.

# commit the megre to branch lab4
$ git commit -am "merged previous lab solutions"

# update the remote tracking branch to your origin repo 
# instead of the upstream repo
$ git push -u origin    
To https://ipads.se.sjtu.edu.cn:2020/[username]/chcore.git
 * [new branch]      lab4 -> lab4
Branch 'lab4' set up to track remote branch 'lab4' from 'origin'.
```

### Building

To build the user programs and kernel, you can type:

```
$ make user # user programs
$ make build bin=xxx # kernel
```

In the above commands, `xxx` is the name of the user program executed by the kernel. For example, if you want to run program `/yield_single.bin` built by `user/lab4/yield_single.c`, the `xxx` should be `yield_single`. **If "bin=`xxx`" is not set, the kernel will run the kernel test in Lab4 and cause a "no given test" bug before it returns to the user mode, which is expected behavior**. You can use `make bin=xxx` to replace the above two commands.

As previous labs, to run the kernel, you can use `make qemu`. To debug the kernel, you can use `make qemu-nox-gdb` and `make gdb`.

We encourage you periodically commit your updates (both on code and document) and push them to our GitLab server, which we have told you in your *ICS* course.

```
$ git commit -am "Exercise xx done!"
$ git push origin lab4
```

### Grading and Hand-In Procedure

**Grading**: coding (80%) + document (20%)

You need to finish all the exercises and questions stated in the lab document.

The total score of Lab 4's test is 100. To check your code grade, type:

```
oslab$ make grade
xxx
xxx
Score: 100/100
```

You should hand-in a document to report how you do the exercises and answer the questions. In lab4, the document is named as `[StudentId]-lab4.md` and stores under the `doc` directory (`mkdir doc` to create it). You can add it to your repo as:

```
oslab$ git add doc/[StudentId]-lab4.pdf
oslab$ git commit -m "add document"
oslab$ git push origin lab4
```

When you finish Lab 4, please submit it with:

```
oslab$ git add.
oslab$ git commit -m"finish lab4"
oslab$ git push origin lab4
```

**Note**: 

1. Except for the documents, you can only modify the files we assign in each exercise. You cannot create any other files and modify scripts. We will overwrite them by origin files when grading. Each `git push`, make sure your lab can be built successfully; otherwise, you will get 0 points.
2. Except for the local test set, we also prepare a remote test set to test your code. The eventual score of your code is combined with both the local test set and the remote test set.

**Please directly contact to the TA, if you:**

1. find any bugs or mistakes in the code or document of the lab4.
2. have any suggestions.

## Part A: Multiprocessor Support

In lab4, the number of processors run by ChCore is extended from 1 to `PLAT_CPU_NUM` (a macro defined in `lib/machine.h`, which stands for the number of available CPUs). To make use of these CPUs, you need to add multiprocessor support in this part. With more processors, you may encounter the concurrency problems, thus locking is used to protect the kernel from multiple CPUs modifying its critical data.

Now, you are going to modify ChCore's boot process to support "Symmetric MultiProcessing" (SMP). In SMP, all CPUs are treated identically when accessing memory, I/O, and other system resources. The only exception is that, during the boot process, the CPUs are classified into two types:

- **Bootstrap Processor (BSP)** who not only initializes itself but also boots the system and OS. Actually, all your previous labs have been running on the BSP. (In the code of the lab, BSP refers to the primary CPU.)
- **Application Processors (APs)** who only initalizes themselves. (In the code of the lab, AP refers to the secondary CPU.)

When booting up, the raspi3 board starts all the CPUs up simultaneously. Then all the CPUs execute the assembly code `_start` in `boot/start.S`, just as previous labs. Although the CPUs are identical, only the BSP is able to make progress in `_start`. Meanwhile, the APs are blocked until the BSP activates them.

> **Question 1:** Read the assembly code `_start` in `boot/start.S`  Use `git diff lab3` to compare the difference of `_start` between previous labs and Lab4. Explain how `_start` determines the BSP and blocks the APs.

### Per-CPU State

When writing a multiprocessor OS, it is important to distinguish between the per-CPU states that are private to each processor, and the global states that the whole system shares. In Lab4, the arrays with `PLAT_CPU_NUM` entries usually store the per-CPU states. The `smp_get_cpu_id()` in `kernel/common/smp.c` always returns the ID of the CPU that calls it, which can be used as an index into the above arrays.

For example, `cpu_status` in `kernel/common/smp.c` contains the flags used for the management of APs' booting, which you will use in the next subpart.

### Application Processor Bootstrap

As previous labs, the BSP executes `main()` in `kernel/main.c` before the first time it returns to the user mode. Directly after the BSP finishes OS initialization (what you implemented in the previous labs),  it activates the APs one by one.

> **Exercise 2:** There is a newly added function `enable_smp_cores()` in `main()`. Read the code of `enable_smp_cores()` in `kernel/common/smp.c` and  the assembly code `_start` in `boot/start.S`.  Fill the C code in `enable_smp_cores()` and `secondary_start()` of `kernel/main.c`, so that the BSP activates the APs one by one. You should pass test `smp` and get the first 5 points!

> **Question 3:** Make sure you understand the control flow of booting APs, which is much like the BSP's. Please answer the question that is it correct for the BSP to activate the APs simultaneously instead of one by one? In other words, will simultaneously booting APs cause concurrent problems? Hints: Check whether each CPU shares the same kernel stack. Hints: Check whether each function call in the control flow can cause race conditions.

### Locking (Big kernel lock)

From now, our code is run on multiple processors, we should first solve the concurrency problems so that the code will not cause unexpected concurrency bugs. In this lab, we just use the simplest way for concurrency control, the big kernel lock (a globally shared lock in the kernel). Directly before the CPUs access any data in the kernel mode, it should first acquire the big kernel lock. Similarly, CPUs should release the big kernel lock before leaving the kernel mode. As a result, only one CPU can run in the kernel at the same time and there are no race conditions.

In `kernel/common/lock.c`, the code is an implementation of the [ticket lock](https://en.wikipedia.org/wiki/Ticket_lock).

> **Exercise 4:** Get familiar with the basic idea of the ticket lock. Then fill the blank the `unlock()` and `is_locked()` in `kernel/common/lock.c`. Attention: there is **no need** to use any assembly code such as the memory barrier and the code you need to write is **less than five lines**. Your code should pass test `mutex` and get 5 points!

ChCore uses the big kernel lock with `lock_kernel()` and `unlock_kernel()` interfaces, which wrapper the global ticket lock.

After ensuring that your lock implementation is correct, we should apply the big kernel lock at 6 locations:

- `main()` in `kernel/main.c`, acquire the lock before the BSP activates the APs so that the BSP gain the big kernel lock first among the CPUs.
- `secondary_start()` in `kernel/main.c`, acquire the lock after the initialization finishes and before the AP returns to user mode.
- `el0_syscall` in `kernel/exception/exception_table.S`, acquire the lock before jumping to the corresponding syscall entry in the `syscall_table`.(We have already implemented the assembly code for you).
- `handle_entry_c` in `kernel/exception/exception.c`, acquire the lock at the first line of the function call. As it is possible for an exception to happen in the kernel mode, you should not acquire the lock if the exception is caught in the kernel.
- `handle_irq` in `kernel/exception/irq.c`, acquire the lock at the first line of the function call. Similar to `handle_entry_c`, you should not acquire the lock if the exception is caught in the kernel.
- `exception_return` in `kernel/exception/exception_table.S`, release the lock in the first line of `exception_return`. `exception_return` is used in all cases where the CPU in kernel mode returns to the user mode. Therefore, it is the only place where you should call `unlock_kernel()`. **This is the only place in Lab 4 where you need to write one line of assembly code**

> **Exercise 5:** Implement the `kernel_lock_init()`, `lock_kernel()`, and `unlock_kernel()` in `kernel/common/lock.c`. Apply the big kernel lock as described above, by calling `lock_kernel()` and `unlock_kernel()` at the proper locations. Your code should pass test `big lock` and get 5 points!.

> **Question 6:** When calling `unlock_kernel()` in `exception_return`, you do not need to save the values of registers such as `x0` to the stack, which is done when `el0_syscall` calls `lock_kernel()`. Why?

## Part B: Scheduling

Currently, you have got a runnable OS, but you are still not able to schedule the multiple threads. You will first implement cooperative scheduling, allowing the CPU to switch from one thread to another when the current thread running on the CPU requires or exits. Later, you will implement preemptive scheduling, which allows the kernel to re-take control of the CPU from a thread after a certain time period, even if the thread does not cooperate. Last, you will extend the scheduler from scheduling the threads of per-CPU to all the CPUs.

### Cooperative Scheduling

In this subpart, you will implement the a basic scheduler which schedules the threads running on the same CPU. This helps you get familiar with the scheduler module in ChCore. All the related data structures can be found in `kernel/sched/sched.h`.

`current_threads` is an array of points to the threads running on each CPU respectively. Different from Lab 3, `current_thread` now is a macro which represents `current_threads[smp_get_cpu_id()]`, although its semantic does not changes. 

`ready_thread_queue` contains the list of ready threads for scheduler. 

`sched_ops` represents the scheduler in ChCore. It stores the function pointers pointing to different scheduling operations in order to support different scheduling policies.

Namely, `sche_init()` initialized the scheduler. `sched_enqueue()` adds a new thread to the scheduler's ready queue. `sched_dequeue()` takes out a thread from the scheduler's ready queue. `sched()` is called when ChCore schedules a thread to execute, it calls enqueues the current thread to the scheduler's ready queue, and then dequeues and selects the head thread of scheduler's ready queue for scheduling. `sched_choose_thread()` is used by `sched()` to choose an available thread to be scheduled. `sched_handle_timer_irq()` is a timer interrupt handler which will be introduced later.

`cur_sched_ops` points to the actual `sched_ops` used by ChCore. (ChCore wrappers the calls to `cur_sched_ops` in static functions defined in `kernel/sched/sched.h` )

#### Round-Robin (RR) Scheduling Policy

In order to schedule the threads in a round-robin fashion, you will implement the `sched_ops` called `rr` in `kernel/sched/policy_rr.c`. Round-robin scheduling in ChCore works as follows:

Each CPU has its own list called `rr_ready_queue`, which stores the ready-to-schedule threads of the CPU. One thread can only appear in one CPU's `rr_ready_queue`. When a CPU calls `rr_sched_enqueue()`, you should enqueue the given thread to the CPU's own `rr_ready_queue` and set the state of the thread to `TS_READY`. Also, when a CPU calls `rr_sched_dequeue()`, you should dequeue the given thread from the CPU's own `rr_ready_queue` and set the state of the thread to `TS_INTER` which stands for the intermediate thread state.

Once a CPU wants to schedule, it only needs to call `rr_sched()`, it first checks whether there is currently running thread. If yes, it calles `rr_sched_enqueue()` to add back the thread to `rr_ready_queue`. Then, it calls the `rr_choose_thread()` to choose the thread to schedule.

When a CPU has no thread to schedule, it should not busy wait and hold the big kernel lock, which would block the entire kernel. Therefore, our round-robin policy creates one idle thread for each CPU. (idle threads are in `idle_threads` of `kernel/sched/policy_rr.c` ) `rr_choose_thread` first checks whether the CPU's `rr_ready_queue` is empty. If yes, `rr_choose_thread` returns the CPU's own idle thread. If not, it selects the head of `rr_ready_queue` and calls `rr_sched_dequeue()` to dequeue the head, and then return the head. `idle_thread` should not appear in the `rr_ready_queue`. Thus both `rr_sched_enqueue()` and `rr_sched_dequeue()` should handle the idle threads specially.

```
rr_sched_init()` is used to initialize the scheduler. It should be called only once in `main()` of `kernel/main.c` and the BSP is responsible to initialize all the entries in `rr_ready_queue` and `idle_threads
```

> **Exercise 7:** Implement the functions in `kernel/sched/policy_rr.c`. Your code should pass test `cooperative` and get 10 points!. Now, your scheduler can work on one CPU.

> **Exercise 8:** Remind that you have acquired the big kernel lock in `handle_irq` of `kernel/exception/irq.c` if the exception is from the user mode. However, there is a special case that if the exception is caught in the idle threads (which run in the kernel mode), where you should also acquire the big kernel lock.
>
> There is no test for this exercise, but you have to finish it. Otherwise, your kernel may block forever. Please think of the reason.

> **Exercise 9:** Now, although your scheduler is not complete, it is ready to run some simple user-mode programs. Implement syscall `sys_get_cpu_id()` in `syscall.c`, which tells the user-mode program the id of the CPU it is running on. Implement syscall `sys_yield()` in `sched.c` which enables the user-mode program to initiate a thread scheduling. Your should be able to run `/yield_single.bin` and get the following output:
>
> ```
> ... 
> Hello, I am thread 0
> Hello, I am thread 1
> Iteration 0, thread 0, cpu 0
> Iteration 0, thread 1, cpu 0
> Iteration 1, thread 0, cpu 0
> Iteration 1, thread 1, cpu 0
> ... 
> ```
>
> As the test scripts first run kernel tests, then the user program tests. Although you can run the user-mode program correctly, you can not run `make grade` to get the 5 points of test `yield single`, and they will be given back later.

### Preemptive Scheduling

With the cooperative scheduler you just implemented, ChCore is able to schedule threads in one CPU. However, if a user-mode thread does not want to give back the control of the kernel, the kernel has no way to force the cooperative to do so. In this part, you will support preemptive scheduling to help the kernel periodically retake the control of the CPU.

#### Clock Interrupts and Preemption

Run the `/yield_spin.bin` user program. This user program's main thread creates one so-called spin thread. Then the spin thread runs an infinite loop once it receives control of the CPU. The problem is that neither the main thread nor the kernel can retake the control of the CPU. Obviously, this is not an ideal situation in terms of protecting the system from bugs or malicious code in user-mode threads. Any user-mode thread can bring the whole system to a halt simply by getting into an infinite loop and never giving back the CPU.

#### Handle the Clock Interrupts

In order to allow the kernel to preempt a running thread, forcefully retaking control of the CPU from it, we must extend the ChCore to support external hardware interrupts from the clock hardware.

The initialization of timer interrupts has already been provided to you and used in `exception_init_per_cpu()` of `kernel/exception/exception.c`. 

For simplicity, we disable interrupts in the kernel mode (except for the idle threads). This is achieved with the help of the hardware. When an exception happens, the interrupt is disabled. When the assembly code `eret` is called, the interrupt is enabled. Therefore, you do not need to worry that the Clock interrupt will interrupt you kernel code. What you need to do is to handle the user-mode timer interrupts.

> **Exercise 10:** Uncomment the code `timer_init()` in `exception_init_per_cpu()`. This will enable the hardware timer interrupt in user mode.  Then you should also modify  `handle_irq()` of `kernel/exception/irq.c` to make the interrupt handling can run normally.
>
> You should be able to get the `/yield_spin.bin` test to work: the main thread should be able to regain the control of the CPU after a certain time and terminate gracefully. Because of the test scripts, you can not run `make grade` to get the 5 points of test `yield spin`, and they will be given back later.

#### Scheduling Budget

In real operating systems, it is usually not worth doing scheduling for every timer interrupt, since the overhead is not negligible if scheduling is invoked too many times.

For each thread, we maintain a `sched_cont` in `kernel/sched/sched.h`. It is actually the budget for each thread. When a timer interrupt is handled, instead of directly scheduling the current thread out, we decrease its budget by one and schedule it out only when its budget is zero. For your scheduling policy, in `sched()`, the scheduler can only schedule out the current thread if its budget is equal to zero. Attention: do not forget to reset the budget of threads.

`rr_sched_handle_timer_irq()`  in `kernel/sched/policy_rr.c` is a handler called when the kernel handles an timer interrupt. You can change the value of the current thread's budget in it.

> **Exercise 11:** Modify your scheduler logic in `kernel/sched/policy_rr.c` so that it can support budget. Follow the above description, implement `rr_sched_handle_timer_irq()`. 
>
> Do not forget to call `sched_handle_timer_irq()` in a proper code location.
>
> Do not to forget to reset the budget in `sys_yield()` in `kernel/sched/sched.c`, so that it can still schedule out the current thread immediately. Your code should pass test `preemptive` and get 5 points!

### Processor Affinity

Up to now, you have implemented a powerful scheduler supporting preemption. However, the round-robin policy in ChCore maintains one `rr_ready_queue` for each CPU, and there is no way to schedule the threads across the CPUs.

To fix this problem, we introduce a concept called "Affinity", the affinity enables the thread to bind to a specific CPU. When creating a thread, you should also specify the thread's affinity. If the value of affinity is set to the id of some CPU, the thread will be added to that CPU's `rr_ready_queue`. Our previous round-robin policy binds the thread to the `rr_ready_queue` of the CPU, which creates the thread. This is also the default behavior when the thread is created with affinity being set to `NO_AFF`.

> **Exercise 12:** Enable your `rr_sched_enqueue()` in `kernel/sched/policy_rr.c` to act depends on the enqueued thread's affinity. If the affinity is set to `NO_AFF`, you should enqueue the thread to the `rr_ready_queue` of the current CPU. Otherwise, enqueue the thread to the `rr_ready_queue` specified by affinity. Your code should pass test `affinity` and get 5 points! 
>
> Up to now, you can run the comprehensive test `sched` for your scheduler. You should pass test `sched` and get 5 points! 
>
> Then, you have passed all the Lab 4 kernel tests. Run `make grade`, you should pass tests `yield single`, `yield spin`, and `yield multi`, and get 15 points!

A thread's affinity can be set in runtime, there is two syscalls called `sys_set_affinity` and `sys_get_affinity`, which are used to set or get the thread's affinity.

> **Exercise 13:** Implement syscall `sys_set_affinity` and `sys_get_affinity` in `thread.c`. You should pass tests `yield aff` and `yield mutli aff`, and get 10 points!

This completes Part B of the lab; make sure it passes all of the Part B tests.

## Part C: Spawn

ChCore now can run the process created in `main()` of `kernel/main.c`. However, a more comprehensive OS should allow the user-mode program to execute a specific file.

When an OS executes given executable binary, it should create a new process responsible for the execution of the file. Remind what you have learned in ICS course. To execute a specific file, the Linux first calls `fork()` then `exec()`. While there is a function called  [spawn](https://en.wikipedia.org/wiki/Spawn_(computing)), which can provide a similar functionality as the combination of `fork()` and `exec()`. In part C, we will implement a user-mode `spawn()`.

### The Interface

Read the code of `spawn()` in `user/lib/spawn.c`. It first gets the ELF (Executable and Linkable Format) of a specific file and passes the ELF to the core function of `spawn()`. In this subpart, you will implement the function called `launch_process_with_pmos_caps()` and then make the `spawn()` runnable. First, we will introduce the interface of the core function.

```
int launch_process_with_pmos_caps(struct user_elf *user_elf,
          int *child_process_cap,
          int *child_main_thread_cap,
          struct pmo_map_request *pmo_map_reqs,
          int nr_pmo_map_reqs,
          int caps[],
          int nr_caps,
          s32 aff)
```

`user_elf` specifies the ELF structure of the specific file to execute. 

`child_process_cap` is the output, which is set to the capability of the created child process if not NULL.

`child_main_thread_cap` is the output, which is set to the capability of the child process's first thread if not NULL.

`pmo_map_reqs` specifies the mapping requests. If the parent process wants to share some memory with the child process.

`nr_pmo_map_reqs` specifies the number of `pmo_map_reqs`.

`caps` specifies the capabilities to transfer. If the parent process wants to transfer some existing capabilities to the child process.

`nr_caps` specifies the number of `caps`.

`aff` is the affinity of the first thread of the child process.

### Basic Workflow

You will first implement a basic `spawn()`. The parent process can create a child process executing a specific file, but the parent process cannot pass any information to the child process and get any output from the `spawn()`. We have already provided you with the code skeleton of `launch_process_with_pmos_caps()`, you need to follow the basic workflow and fill in the blanks in the code. ![spawn](../../../../../../../.\image\spawn.svg)

The basic workflow is as follow:

1. Create a child process with `sys_create_process()`. 
2. Map each segment in the binary elf to the child process. 
3. Create a new physical memory object (PMO)`main_stack_pmo` with `sys_create_pmo()`. The PMO will be used as the child process's stack. The PMO's size is `MAIN_THREAD_STACK_SIZE`, and it should be allocated immediately.
4. Construct (prepare) one initial page in parent's local memory, which stores arguments like `argc`, `argv`.
5. Write the  page to the top page `main_stack_pmo` with `sys_write_pmos()`.
6. Map the `main_stack_pmo` to child process using `sys_write_pmo()`.

  It should be mapped to the child process's address of `MAIN_THREAD_STACK_BASE` with `VM_READ` and `VM_WRITE` permissions.

1. Create the main (first) thread of the child process. When the main thread is executed, it starts its thread from `sp`.

> **Exercise 14:**  Implement `spawn()` in `user/lib/spawn.c`. You should pass test `spawn basic`, and get 10 points! Attention: you are not required to do large amounts of coding, just replace all the LAB4_SPAWN_BLANK with correct expression.

### Support transfer pmos and caps for spawn()

You will implement the whole `spawn()`. The parent process is able to pass shared memory and transfer useful capabilities to the child process. The parameters you need to handle are `pmo_map_reqs`, `nr_pmo_map_reqs`, `caps`, and `nr_caps`.

Before the step 4 of basic workflow, you should do following steps:

  A. If `nr_pmo_map_reqs>0`, then call `sys_map_pmos()` with `nr_pmo_map_reqs`.

  B. If `nr_caps>0`, then call `usys_transfer_caps()` with `caps`.

After step 7 of the basic workflow, you should:

  C. Set `child_process_cap` and `child_main_thread_cap` if not NULL.

> **Exercise 15:**  Implement `spawn()` in `user/lib/spawn.c`. You should pass test `spawn info`, and get 5 points!. Attention: you are not required to do large amounts of coding; the total number of code should be no more than **20 lines**.

## Part D: Inter-Process Communication (IPC)

Although `spawn()` enables the parent and child process to share the same memory and communicate with each other, if two processes have no parent-child relationship, they cannot communicate with each other. In the final part, we will modify ChCore to support a more general approach for the processes to pass messages to each other.

### IPC in ChCore

ChCore's IPC interfaces are not the traditional message passing send/recv interfaces. It is like a client/server model where the IPC request receiver is a server, and the IPC request sender is a client.

To get ready for serving the IPCs, the server needs to register its callback function `ipc_dispatcher()` first and should never exit if it wants to serve the client's IPC requests. ChCore uses `ipc_connection` in `ipc/ipc.h` to bind a client to the server. For each `ipc_connection`, the kernel creates a user-mode server thread responsible for executing the server's `ipc_dispatcher()`. The thread resides in the server's process; thus, the thread shares the same `vm_sapce` with the server process.

To start an IPC request, the client should first create an `ipc_connection` to the server. Then it can send an IPC to the server using specific `ipc_connection`. When the kernel is handling `sys_ipc_call()` for IPC, it will migrate the client thread's execution (not context) to server thread (We call this procedure thread migration). Then the kernel returns to the server thread to execute the server's `ipc_dispatcher()`.  After the server thread gets the results and call `sys_ipc_return()`, the kernel will migrate the server thread's execution back to the client thread with the return value.

![ipc](../../../../../../../.\image\ipc.svg)

### System Calls for IPC

- `sys_ipc_register_server()` is used by the server process to register an IPC server.

Its parameters are the address of `ipc_dispatcher()`, the maximum number of servable clients, and the `server_vm_config`. `server_vm_config` specifies the virtual address (in the server process) and size of 1) the stacks used by threads of `ipc_connection` and 2) the shared memory buffer used between clients and the server.

- `sys_register_client()` is used by a client process to create an `ipc_connection` from the caller thread to the target server.

Its parameters are the capability of the server process and the `client_vm_config`. `client_vm_config` specifies the virtual address (in the client process) and size of the shared memory buffer used between clients and the server. Its return value is the capability of the `ipc_connection`.

- `sys_ipc_call()` is used by a client process to issue an IPC request.

Its parameters are the capabilities to the `ipc_connection` and the `ipc_msg`. `ipc_msg` actually resides on the start address of the per-connection shared memory buffer between the client and server. For small messages, the client can directly use the buffer to transfer data. While for large messages, the client needs to create PMOs and store the capabilities of PMOs on the buffer to transfer the shared memory. `kernel/ipc/ipc.h` provides the details of `ipc_msg`.

- `sys_ipc_return()` is used by the thread of `ipc_connection`.

Its parameter is the 64-bit value returned by the registered `ipc_dispatcher()` in the server thread.

> **Exercise16:** We have provided you with most code of IPC related syscalls in `kernel/ipc/`. Please fill the rest according to the comments in the file. You should pass tests ipc data and ipc mem, and get 15 points!. Attention: you are not required to do large amounts of coding, just replace all the LAB4_IPC_BLANK with correct expression.

> **Exercise17:** Be sure you have understood the workflow of IPC. In the last exercise, you need to implement a simplified syscall called `sys_ipc_reg_call()`, which is much similar to `sys_ipc_call()`. The only difference is that the second argument of `sys_ipc_reg_call()` is a 64-bit value instead of the address to the shared memory buffer. The second argument should be transferred to the server thread directly, as the only parameter of `ipc_dispatcher()`. You should pass test `ipc reg`, and get 5 points!

### Send/Recv IPC

This subpart is for students who want to achieve the bonus. You can ignore this subpart and bonus exercise if you are not interested in the bonus.

Some systems use `send()` and `recv()` as their IPC interface. Can you design and implement a send/recv IPC for ChCore? A receiver thread calls `recv()` and then block until any sender thread calls `send()` and sends IPC msg to the receiver thread. 

**Bonus:** Create a new branch called lab4-bonus to finish this bonus exercise when you finish all the previous exercises and questions. Then you can implement `ipc_send()` and `ipc_recv()` for ChCore. As you are in the new branch, you can modify any code as you wish. 

Following are some announcements:

- You need to give one user-mode program (one is enough) to test your send/recv IPC. You can code the program based on any test like `ipc reg`.

- You need to only transfer 64-bit value like `ipc_reg_call`. Supporting shared memory message passing is unnecessary.

- You can not implement a user-mode wrapper of `ipc_call()` and `ipc_return()`.

- You can reuse  `ipc_register_server()` and `ipc_register_client()` if needed.

- There is no limitation on the return value or parameter of your `ipc_send()` and `ipc_recv()`.

- There is no limitation on how you modify the kernel code.

- You should describe how you design and implement the send/recv IPC. It is recommended to give brief but complete descriptions instead of lots of coding details.

	

**This completes the lab.** Make sure you pass all of the tests and get the total scores of 100.

Don't forget to write up your answers to the questions.

 