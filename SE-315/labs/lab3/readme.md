# ChCore Lab 3: User Processes

**Hand out: 2020-04-03**

**Deadline: 2020-04-29 23:59**

**TA in charge: ZHAO Ziming (dumplings_ming \at 126.com)** 

## Introduction

In this lab you will implement the basic kernel facilities required to get a protected user-mode process  running. You will enhance the ChCore kernel to set up the data structures to keep track of user processes, create a single-thread user process, load a program image into it, and make it running. You will also make the ChCore kernel capable of handling any system calls the user process makes and handling some other exceptions it causes.

### Getting Started

Use Git to commit your changes after your Lab 2 submission (if any), fetch the latest version of the `upstream` repository. Then create a local branch called `lab3` based on our upstream lab3 branch, `upstream/lab3`. Finally merge your local lab changes to your local lab3 branch and sync your local lab3 branch with your own remote lab 3 branch `origin/lab3`:

```
$ git commit -am 'my solution to lab2'
$ git fetch upstream
$ git checkout -b lab3 upstream/lab3
Branch lab3 set up to track remote branch refs/remotes/upstream/lab3.
Switched to a new branch "lab3"
$ git merge lab2
Merge made by recursive.
$ git commit -am "merged previous lab solutions"
$ git push -u origin lab3
Branch 'lab3' set up to track remote branch 'lab3' from 'origin'.
```

Here you have moved all your code into lab3.

### Lab Requirements

This lab is divided into three parts, A, B, and C. You should commit your changes and hand in your lab before the deadline, even though your code may not yet pass all of the grade script tests.

As in previous labs, both the answers to the questions in this lab and a brief explanation of what you have done to finish the exercises in the lab should be put into a document pdf or markdown file called `[StudentId]-lab3.pdf` or `[StudentId]-lab3.md` under the`doc` directory. Don't forget to add it to your git repo with

```
$ git add doc/[StudentId]-lab3.pdf
$ git commit -m "add document"
$ git push origin lab3
```

We encourage you usually commit your updates (both on code and document) and push them to our GitLab server, just like previous labs.

```
$ git commit -am "Exercise xx done!"
$ git push origin lab3
```

Before your first run or after you have modified some code in the `user/` folder, use the following command to build all user programs:

```
$ make user
```

To check you code grade, you can type:

```
$ make grade
```

To run a specific user program, which may be very helpful in the following exercises, use:

```
$ make run-X
```

Where X is the user program's name, like `make run-hello`. **NOTE:** `make run-X-nox` ,`make run-X-gdb` , and `make run-X-gdb-nox` also exist in this lab, which have similar semantics as pervious labs.

### Inline Assembly

In this lab, you may find GCC's inline assembly language feature useful. At the very least, you will need to be able to understand the fragments of inline assembly language ("`asm`" statements) that already exist in the source code we gave you. You can find several sources of information on GCC inline assembly language on [this document](https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html) page.

## Part A: User Processes

### AArch64 Exception Levels

AArch64 architecture defines software execution privilege levels using the concept **Exception Level**. Four exceptions levels, EL0 to EL3, are defined where EL3 is the highest exception level with the most execution privilege.

The following is a common usage model for the exception levels:

- **EL0**: User-space applications.
- **EL1**: OS kernels and associated functions that are typically described as privileged.
- **EL2**: Hypervisor.
- **EL3**: Secure monitor.

ChCore only uses exception level El0 and El1. EL1 represents for kernel-mode in ChCore, at which all kernel codes in directory `kernel/` are executing. EL0 represents for user-mode, at which the user code in the `user/` folder should be working.

### Capability-based Access Control

 In ChCore, all kernel resources exposed to applications are managed by *capability mechanism* ([wiki](https://en.wikipedia.org/wiki/Capability-based_security)). Applications can access a kernel **object** through an integer identifier (**cap**). To make an analogy with file descriptor in Linux, a file descriptor is a cap, and the file itself is a kernel object. The mapping relationship of a cap to an object is identical for all threads within the same process.  

You are not supposed to implement any logic of capability mechanism. But you should be familiar with the interface to allocate cap and object, and how to query an object with cap. We use two functions called `sys_create_pmo` and `sys_map_pmo` as examples. `pmo` (physical memory object) is a kernel object representing a chunk of physical memory.

```
/*
 * Cap and object allocation. Error handling is omitted.
 * This function allocate a pmo object and return cap to use.
 */
int sys_create_pmo(u64 size, u64 type)
{
        int pmo_cap;
        struct pmobject *pmo;

        /* Allocate an object with type(TYPE_PMO) and size(sizeof(*pmo)) */
        pmo = obj_alloc(TYPE_PMO, sizeof(*pmo));
        /* Do whatever you want to init the object */
        pmo_init(pmo, type, size, 0);
        /* Allocate an cap to pmo. */
        pmo_cap = cap_alloc(current_process, pmo, 0);
        return pmo_cap;
}

/*
 * Query an object with cap. Error handling is omitted.
 * Using cap returned by sys_create_pmo, you can query a pmo object.
 */
int sys_map_pmo(u64 process_cap, u64 pmo_cap, u64 addr, u64 perm)
{
        struct pmobject *pmo;
  
        /* Get pmo object by its cap and type(TYPE_PMO) */
            pmo = obj_get(current_process, pmo_cap, TYPE_PMO);
        /* Use pmo */
        ......
        /*
         * Finish the use of pmo object. This might be eccentric for 
         * you now and you don not need to understand it. It is for
         * correctness of concurrent access.
         */
        obj_put(pmo);
        ......
}


```

Complete interfaces are listed in `kernel/process/capability.h` including allocate, copy, delete, revoke of capability, but is beyond the scope of this lab. 

### Process and Thread

ChCore kernel now uses the `process` data structure to keep track of processes and use the `thread` data structure to keep track of threads. In ChCore, a process is a group of threads sharing the same capabilities to a set of kernel objects. Like Linux, each process in ChCore may contain multiple threads sharing the same set of capabilities, while each thread in ChCore must belong to one and only one process. The new include files `kernel/process/process.h` and `kernel/process/thread.h` contains the definition for those two data structures.

The `process` structure is defined in `kernel/process/process.h` as follows:

```
struct process {
    struct slot_table slot_table;
    struct list_head thread_list;
};
```

Here's what each field are for:

- `slot_table`: A array of kernel objects to which this process have access right
- `thread_list`: A linked list of threads belonging to this process

The `thread` structure is defined in `kernel/process/thread.h` as follows:

```
struct thread {
    struct list_head    node;
    struct thread_ctx   *thread_ctx;
    struct vmspace      *vmspace;

    struct process      *process;
};
```

Here's what each field are for:

- `node`: A linked list of threads belonging to the same process of this thread
- `thread_ctx`: Scheduling context of this thread. Currently, it only contains all registers of current thread and the type of current thread.
- `vmspace`: The mapped virtual memory space of current thread
- `process`: The process this thread belongs to

In this lab, you will create just one user process containing one single thread. Only one single CPU with ID 0 is enabled in this lab to run that process. Other messy details related to the scheduler and multiple CPUs will be implemented in Lab4.

### Creating and Executing Main Thread

You will now write the code in `kernel/process/thread.c` and `kernel/sched/context.c` necessary to run a user process with only one thread on CPU0. Because we do not yet have a filesystem, we will set up the kernel to load a static binary image that is *embedded within the kernel itself*. ChCore combines all executable elf images compiled from `user/*` into a single file with the help of `cpio` and embeds that file into the kernel at variable `binary_cpio_bin_start`. Checkout `user/binary_include.S` and `scripts/compile_user.sh` for more details. To compile all user programs in the `user/` directory, use `make user` as described before.

In `main()` in `kernel/main.c` you'll see code to run one of these binary images defined by the macro `TEST` in the root process. However, the critical functions to set up a thread and load an elf format binary are not complete; you will need to fill them in.
m
Below is a call graph of the code up to the point where the user code is invoked. Make sure you are familiar with them. 

- `start`
- `main (kernel/main.c)`
  - `uart_init`
  - `mm_init`
  - `exception_init`(not complete yet)
  - `process_create_root`
    - `process_create`
    - `thread_create_main`
  - `eret_to_thread`
    - `switch_context`

Among the full call graph, the function `eret_to_thread` defined in file `kernel/exception/exception_table.S` is the most important one. It uses instruction `eret` to start running code in the user mode. The semantic of instruction `eret` includes:

- Check the value in the current exception level's `SPSR` register (`SPSR_EL1` in our case) to ensure the target exception level of the processor after the `eret` instruction (`EL0` in our case).
- Set up the  `PC` registers  according to `ELR_ELx` (exception return address pointer for `ELx`) for current exception level (`EL1` in our case), which is `ELR_EL1`.
- Set up the value of `PSTATE` registers according to `SPSR` register of current exception level.
- Switch to the target exception level and begin the execution. (NOTE: after the exception level switch, the `SP_ELx` of target exception level (`SP_EL0` in our case) is used as the `SP` register instead of the `SP_ELx` register of source exception level (`SP_EL1` in our case)).

When running a thread, the following memory regions are necessary:

- User stack: created in `thread_create_main`, is a small region of memory used as the stack of the user thread.
- Kernel stack: created in `create_thread_ctx`, is a small region of memory to be used as the stack of the corresponding kernel thread (current thread when executing at kernel mode). The data structure `thread_ctx` is stored on the kernel stack of the current thread.
- Elf code/data regions: created in `load_binary`, are a set of memory regions used to hold code/data segments loaded from the elf file.
- User heap: created by the `sys_handle_brk` system call, is a small region of memory that will be lazy allocated at the first access time. To be used as user processes' heap.

**Exercise 1.** Find and finish the following functions:

- `load_binary`: Parse an ELF binary image, and load its contents into the user address space of a new thread. Also check the `lib/elf.h` and the [elf wiki](https://en.wikipedia.org/wiki/Executable_and_Linkable_Format) for help.
- `init_thread_ctx`: Initialize the context of a thread, so that later `main` can know how to run this thread accordingly.
- `switch_context`: Switch to the context of the current thread. The return value of this function is used as `eret_to_thread(switch_context())`. 

**Exercise 2.** Please briefly describe what `process_create_root` has done. **NOTE**: a detailed description of the logic in function `thread_create_main` is required and drawing call graphs is recommended.

Once you are done, you should compile your kernel and run it under QEMU with command `make run-hello`. For now, since many codes a not finished yet, you can temporarily remove the `-Werror` tag in the `CMakeLists.txt`. However, after you have finished all this lab, don't forget to add that back since we are going to build your lab3 with that tag when grading. 

If all goes well, your system should enter user mode and execute the hello binary until it makes a system call with the `SVC #0` instruction. There will be a trouble since ChCore has not set up the hardware for an entry point of handling any transition from user mode (`EL0`) into the kernel mode (`EL1`). When trying to execute the `SVC` instruction, ChCore will execute code at some random location and then trigger an *Instruction Abort*. Later, the *Instruction Abort* still has no entry point specified, so the ChCore continues this loop and finally show up as stalled.

We'll address this problem shortly, but for now, we can use the debugger to check that we're entering user mode. Use command `make run-hello-gdb` and then `make gdb` to attach a GDB to the QEMU. When entered GDB, try to use `b START` to break at the entry point of the user program. Later, continue running QEMU. If your breakpoint at the user program entry point has been triggered, that means you have successfully entered the user mode.

## Part B: Exception Handling

At this point, the first `svc #0` system call instruction in user mode is a dead end: once the processor gets into user mode, there is no way to get back out. You will now need to implement basic exception and system call handling, so that it is possible for the kernel to recover control of the processor from user-mode code. The first thing you should do is thoroughly familiarize yourself with the arm exception handling mechanism.

Please read *Chapter 10. AArch64 Exception Handling* in [ Programmer’s Guide for ARMv8-A ](https://static.docs.arm.com/den0024/a/DEN0024A_v8_architecture_PG.pdf?_ga=2.178331556.103194251.1582861787-1806723973.1581424534) to get yourself familiar with Arm's exception and interrupt handling process.

Generally speaking, terms such as exception, trap, interrupt, fault and abort have no standard meaning across architectures or operating systems, and are often used without regard to the subtle distinctions between them on a particular architecture such as the AArch64. In this Lab, we follow ARM's terminology for exceptions, interrupts, abort, and the like.

### AArch64 Exceptions

Strictly speaking, an interrupt is something that interrupts the flow of software execution. However, in ARM terminology, that is actually an exception. Exceptions are conditions or system events that require some action by privileged software (e.g. an exception handler inside kernel) to ensure smooth functioning of the system.

In AArch64, exceptions can be either synchronous, or asynchronous.

- An exception is described as being synchronous if it is generated by direct execution of instructions and the return address indicates the instruction which caused it. 
- Otherwise, the Exception is described as asynchronous.

Source of synchronous exceptions are synchronous aborts and exception generating instructions. Synchronous aborts are generated by failed instruction fetches or failed data accesses when directly executing one instruction. Exception generating instructions are  instructions that can generate exceptions. Those instructions, including `SVC`, `HVC`, and `SMC`, are typically executed to request a service from software that runs at a higher privilege level.

Sources of asynchronous exceptions are `IRQ`, `FIQ`, or `SError` (System Error). `IRQ` and `FIQ` are general purpose interrupts generated by other hardware connected to the processor, such as a timer interrupt. System errors have several possible causes, the most common being asynchronous Data Aborts (for example, an abort that is triggered by write-back of dirty data from a cache line to external memory).

### Exception Vector Table

When an exception occurs, the processor must execute handler code which corresponds to the exception. The location in memory where the handler code is stored is called the `Exception Vector`. In the ARM architecture, exception vectors are stored in the `Exception Vector Table`. Each Exception level has its own vector table, the virtual address of which is set by the `Vector Based Address Register`s (`VBAR_EL3`, `VBAR_EL2` and `VBAR_EL1`). The table contains instructions to be executed, rather than a set of addresses. Vectors for individual exceptions are located at fixed offsets from the beginning of the table. 

 Which entry is used depends upon a number of factors:

- The type of exception (`SError`, `FIQ`, `IRQ` or `Synchronous`)
- If the exception is being triggered at the same Exception level, the Stack Pointer to be used (e.g. when a exception is triggered in `EL2`, the exception handler can use either stack pointer for `EL0` (`SP_EL0`) or `EL2` (`SP_EL2`) according to the `SPSel` register in the processer). 
- If the exception is being triggered at a lower Exception level, the execution state of the next lower level (AArch64 or AArch32)

Offsets of each exception vector in the table are listed below:

| Address（based from VBAR_ELn） | Exception type | Other                  |
| :----------------------------- | :------------- | :--------------------- |
| `+ 0x000`                      | Synchronous    | Current EL with SP_EL0 |
| `+ 0x080`                      | IRQ/vIRQ       |                        |
| `+ 0x100`                      | FIQ/vFIQ       |                        |
| `+ 0x180`                      | SError/vSError |                        |
| `+ 0x200`                      | Synchronous    | Current EL with SP_ELx |
| `+ 0x280`                      | IRQ/vIRQ       |                        |
| `+ 0x300`                      | FIQ/vFIQ       |                        |
| `+ 0x380`                      | SError/vSError |                        |
| `+ 0x400`                      | Synchronous    | Lower EL using AArch64 |
| `+ 0x480`                      | IRQ/vIRQ       |                        |
| `+ 0x500`                      | FIQ/vFIQ       |                        |
| `+ 0x580`                      | SError/vSError |                        |
| `+ 0x600`                      | Synchronous    | Lower EL using AArch32 |
| `+ 0x680`                      | IRQ/vIRQ       |                        |
| `+ 0x700`                      | FIQ/vFIQ       |                        |
| `+ 0x780`                      | SError/vSError |                        |

For example, if kernel code is executing at `EL1` and an IRQ interrupt is signaled, an interrupt occurs. This particular interrupt is not associated with the hypervisor or secure environment and is also handled within the kernel, also at `EL1`. The `SPSel` bit is set, so you are using `SP_EL1`. Execution is therefore from address `VBAR_EL1 + 0x280` (Current EL with SP_ELx).

### Exception Handling Registers

Later in this Lab, we mainly focused on handling synchronous exceptions like page faults and system calls. Handling asynchronous exceptions are left for later labs.

When handling synchronous exceptions, registers are used to supply information to exception handlers about the cause of a synchronous exception.

- **Exception Syndrome Register** (`ESR_ELn`): Information about the reasons for the exception. More details of the usage of bits in this register can be found [here](https://developer.arm.com/docs/den0024/a/aarch64-exception-handling/synchronous-and-asynchronous-exceptions/the-exception-syndrome-register) in ARMv8 programmer's guide.
- **Fault Address Register** (`FAR_ELn`): The faulting virtual address for all synchronous instruction and data aborts and alignment faults. E.g. the page fault virtual address is stored in this register. 
- **Exception Link Register** (`ELR_ELn`):  The return address which is preferred for the specific exception type. For some synchronous exceptions (e.g. `SVC`), this is the address of the next instruction after the one which generated the exception. For others, it contains the instruction that generated the exception for easier re-execution. For asynchronous exceptions, the `ELR_ELn` points to the address of the first instruction that has not been executed or executed fully, as a result of taking the interrupt. 

**NOTE**: The Exception Syndrome Register is not updated for IRQ or FIQ as these interrupt handlers typically obtain status information from registers in the *Generic Interrupt Controller* (GIC). More details for supporting IRQ will be described in the coming labs.

### An Example

Let's put these pieces together and trace through an example. Let's say the processor is executing code in a user thread and encounters a instruction that hasn't been defined in ARMv8.

1. The processer puts the cause of the exception into `ESR_EL1`, stores the `PSTATE` to `SPSR_EL1` and puts the return address (also the address of the undefined instruction) into `ELR_EL1`.
2. The processer switches to kernel mode (`EL1`) and the kernel stack pointer (stored in  `SP_EL1`) is used as stack after the mode switch.
3. The processer checks the `VBAR_EL1` to get the address of Exception Vector Table used in EL1. The processer looks up the Exception Vector table. Since current exception is a synchronous exception comes from EL0 executing at AArch64 mode, the processor chooses entry `VBAR_EL1 + 0x400`.
4. The processer executes the code at `VBAR_EL1 + 0x400`, which in our case is a `b` instruction jumping to the exception handler.

### Setting Up the Exception Vector Table

You should now have the basic information you need in order to set up the exception vector table and handle exceptions in ChCore.

In ChCore, since only EL1 are used as kernel mode, we only need to initialize the exception handler for EL1. At this moment, all synchronize exceptions, except for system calls, are passed to `handle_entry_c` and handled by that handler. When an exception is encountered, ChCore will check the exception vector table, pass the exception type and current exception handler entry type as argument using assembly code and finally call `handler_entry_c` to handle the exception in C.

**Exercise 3.** Edit `exception_table.S` and `exception.c` to implement the features described above. You should：

- Fill in the exception vector table in `exception_table.S` with the help of some macros in that file.
- Complete the `exception_init` function in `exception.c` to set up the exception vector after kernel have been booted.
- Handle bad instruction exception by let the kernel print info of macro `UNKNOWN` defined in `esr.h` with `kinfo(UNKNOWN)` and call `sys_exit(-ESUPPORT)` in the handler to let the user thread exit, since such kind of exception is unable to be recovered.

Test your exception handling code using some of the test programs in the `user/lab3` directory that cause exceptions before making any system calls, such as `user/lab3/badinsn.c`. Try them out using `make run-badinsn` and you should be able to see the kernel trapped in a dead loop printing:

```
[INFO] [ChCore] interrupt init finished
[INFO] [ChCore] root thread init finished
[INFO] UnKnown
[INFO] sys_exit with value -12
[ChCore] Lab stalling ...
[INFO] UnKnown
[INFO] sys_exit with value -12
[ChCore] Lab stalling ...
[INFO] UnKnown
[INFO] sys_exit with value -12
[ChCore] Lab stalling ...
[INFO] UnKnown
[INFO] sys_exit with value -12
[ChCore] Lab stalling ...
[INFO] UnKnown
[INFO] sys_exit with value -12
[ChCore] Lab stalling ...
```

Normally, `sys_exit` will delete current thread, find the next runnable thread and switch context. However, since we only have one thread now, the `sys_exit` have no context to switch. So the exception handler will still return to the fault address stored in `ELR_EL1` after the `sys_exit`, which caused a dead loop of bad instruction exception. Don't worry, we called `breakpoint()` in the `sys_exit` and our grading script will kill the kernel at that point. For your convenience, you can call `BUG_ON(1)` in the `sys_exit` while coding. Don't forget to remove that modification before you finally grading your lab.

You should be able to get `make grade` succeed on the `badinsn` and `badinsn2` tests at this point.

## Part C: System Calls and Page Faults

Now that your kernel has basic exception handling capabilities, you will refine it to provide important operating system primitives that depend on exception handling.

### System calls

User processes ask the kernel to do things for them by invoking system calls. When the user process invokes a system call, the processor enters kernel mode, the processor and the kernel cooperate to save the user process's state, the kernel executes appropriate code to carry out the system call and then resumes the user process. The exact details of how the user process gets the kernel's attention and how it specifies which call it wants to execute vary from system to system.

**Exercise 4.** Dislike other exceptions in ChCore, system calls are handled by directly jump to the corresponding system call entry in the `syscall_table` using assembly code. Please read the code in `exception_table.S` and briefly describe how a system call is dispatched from the exception vector table to a entry of the system call table in your document.

**Exercise 5.** Complete `syscall` in `/user/lib/syscall.c` which use `SVC` instruction to trap into kernel and execute the system call. Don't forget to `make user` after your modification.

**Exercise 6.** Finish the following system calls in ChCore.

- `sys_putc`: Use `uart_send` to put a char onto the screen. This is a basic system call used by `printf`.
- `sys_exit`: The helper function `sys_exit` will exit a thread as we mentioned before in exercise 3. Now, you can dispatch corresponding system call to `sys_exit`.
- `sys_create_pmo`: The implementation of this function is in `vm_syscall.c` and will be used later to  test page fault.
- `sys_map_pmo`: The implementation of this function is in `vm_syscall.c` and will be used later to test page fault.
- (**Bonus**) `sys_handle_brk`: user thread will use `sys_handle_brk` to create or expand the user heap.  In this system call, the heap of the current function will be enlarged to the virtual address `addr`. More details of this function can be found in the comment of `vm_syscall.c`. If you are not going to finish this function, don't forget to remove its current existing code to eliminate warnings.

However, at this point, your system is still unable to finish any user program gracefully. This problem will be addressed in the following section. However, you can still test your implementation by running corresponding test programs in the `user/lab3/` folder. For example, use `make run-hello` to run the `/user/lab3/hello` program with your kernel. It should print "`Hello world`" on the console and stalled for now.

### Exiting User thread

The entry point of a user's elf file is assigned according to the `CMakelist.txt` in `user/` folder. It is loaded to a thread's initial `PC` by the `load_binary` and `init_thread_ctx` functions implemented in Part A. Now, the entry point of a user program is the function `START` in `/user/lib/libmain.s`, which directly calls the `start_c` function and calls the user's `main` function finally.

**Exercise 7.** Please check the execution flow after the `ret` instruction in function `START` has been executed with GDB. Try to explain this phenomenon briefly in your document.

**Exercise 8.** Add call to `sys_exit` system call in `/user/lib/libmain.c` to let the main thread exit gracefully.

Generally speaking, when a thread has been exited, the scheduler will choose the next thread to be executed. However, as described in exercise 3, since we now have only one thread, our `sys_exit` will still return to the original thread and cause some exceptions. The call to `break_point` and our grading script can handle this problem.

You should now be able to get make grade to succeed on the `testputc`, `testcreatepmo`, `testmappmo`, `testmappmoerr`. If you have added `BUG_ON` in function `sys_exit` in the previous section, don't forget to remove it before grading.

### Handling Page Faults

The page fault exception, error code 36 (lower exception level) or 37 (current exception level), is particularly notable in an operating system for lazy page allocation or memory protection usage. When the processor takes a page fault, it stores the virtual address that caused the fault in register `FAR_ELx`. In `exception/pgfault.c` we have provided the beginnings of a special function, `do_page_fault()`, to handle page fault exceptions.

**Exercise 9.** Modify `handle_entry_c()` to dispatch page fault exceptions to `do_page_fault()` and finish the `do_page_fault()` function.

You should now be able to get make grade to succeed on the `faultread`, `faultwrite`,  `testpf`, and `testsbrk` (if bonus) tests. If any of them don't work, figure out why and fix it.

## Hand in

**This completes the lab.** Don't forget to remove your `BUG_ON` in `sys_exit` and add the `-Werror` back in the `CMakeLists.txt` before final grading. Write up your answers and descriptions in `[student-id]-lab3.pdf` or `[student-id]-lab3.md`.  To hand in, just commit all your modifications to your git repo and push your local updates to the remote repo.

```
$ git add doc/[StudentId]-lab3.pdf
$ git add -u
$ git commit -m "Lab3 finished"
$ git push origin lab3
```