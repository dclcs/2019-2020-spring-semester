# SE-315

## Lab 3 Report

### Part A: User Processes

#### Exercise 1

We should fill in 3 blanks in this exercise.

##### Phase 1: `load_binary`

First, the `load_binary` blank is quite simple. Grabbing `p_vaddr` and `p_memsz` from `struct elf_program_header`, we can set variable `p_vaddr` and `seg_sz`. Rounding up `seg_sz` by page size, we can get `seg_map_sz`.

After that, we should copy loadable segment to our vmspace. But we haven't implement any file system stuff, so the binary data comes from `load_binary`'s parameter `const char *bin`.

Copy `seg_sz` bytes from `const char *bin` to `pmo->start` should complete this work.

##### Phase 2: `init_thread_ctx`

In this phase, we should set three registers of user thread: `SP_EL0` for stack, `ELR_EL1` for entrypoint, and `SPSR_EL1` for state recovery.

Notice that we shouldn't directly write them into physical registers by `mrs` assembly, since we're still in kernel mode until we call `eret`. So currently we just put them into `thread->thread_ctx->ec.reg` array.

##### Phase 3: `switch_context`

The main challenge is that we should find a context pointer for `eret_to_thread`. Reading codes from `eret_to_thread` (actualy, the `exception_exit` macro), we notice that it loads 35 consecutive `u64` values, putting them into general and special registers.

So, the base address should be `thread->thread_ctx->ec.reg`. Return that in `switch_context` should be fine.

#### Exercise 2

`process_create_root` gets the executable file's name as its parameter. In this case, `hello`.

##### Phase 1: `ramdisk_read_file`

First, it calls `ramdisk_read_file`, which calls `cpio_extract_single` function. This function parses `cpio` archived file, and copies our willing part to a buffer. When we execute `make user`, those binaries are actually archived into one single `cpio` file.

##### Phase 2: `process_create`

After that, it calls `process_create`, which allocates memories, `vmspace`, `slot_id`, and slots for `struct process`. Just a normal initialization. After that, a process is created.

However, currently there's no any thread (including the main thread) in our process. Since all process should have at least one thread, we should create one as the main thread.

##### Phase 3: `thread_create_main`

So, we call `thread_create_main`, creating a new thread in our baby process.

First, we should allocate a chunk of memory as the user stack (`process.c:[274:287]`). After that, we should map those memory addresses by calling `vmspace_map_range` (`process.c:289`).

Then, we should allocate a `thread` structure, filling some critical properties (including `stack`, `pc`, `prio`, `type`, and `aff`). Those properties will be passed to function `thread_init` and later `init_thread_ctx`, which will set them well. After that, we should add our new thread into node list (`thread.c:44`) for later scheduling.

Finally, `thread_create_main` returns to `process_create_root` after lots of hard work.

##### Phase 4: Outro

Finally, we set the `root_thread` (the initial thread) as our `current_thread`, waiting for scheduling.

### Part B: Exception Handling

#### Exercise 3

All exception handler's label has been defined in `exception_table.S`. So we just need to arrange the table by macro `exception_entry`.

After that, in `exception_init` function, we call assembly function `set_exception_vector()` to set the exception vector table register.

After such operation, the control flow will arrive `handle_entry_c` when bad instruction occurred.

Take `ESR_EL1_EC_IABT_LEL` into `switch` expression should handle that.

### Part C: System Calls and Page Faults

#### Exercise 4

Firstly, if we gets a `el0_syscall` exception, the control flow will falls into `exception_table.S:180` by judging if `x24` is `#ESR_EL1_EC_SVC_64`. After storing general registers into stack, we puts `syscall_table` entry into `x27`, and puts `syscall_num` into `x16`, then calculates `[x27, x16, lsl #3]` to get the address of the `x16`th entry of the `syscall_table`, and put it into `x16`. Then, we can use `blr x16` to perform the system call.

#### Exercise 5

Just follow the instructions.

Especially, if we want to add a readable variable in `__asm__`, we should define it with `+`. If we want to make it write-only, define it with `=`.

#### Exercise 6

There's a special C syntax for array initialization.

```c
const void *syscall_table[NR_SYSCALL] = {
	[0 ... NR_SYSCALL - 1] = sys_debug,
	/* lab3 syscalls finished */
	[SYS_putc] = sys_putc,
	[SYS_exit] = sys_exit,
	[SYS_create_pmo] = sys_create_pmo,
	[SYS_map_pmo] = sys_map_pmo,
	[SYS_handle_brk] = sys_handle_brk,
};

```

which seems quite weird. The later a equation is declared, the higher priority it contains.

#### Exercise 7

Currently, when we call `ret` in the user function, it will touch the end of `__start_c` function again. This function doesn't do anythign further, so it will goes to its parent function `START`, which also touches its end. Now the whole executing flow is over and no more code can be executed, also it can't go back to kernel mode to give any further scheduling.

#### Exercise 8

Added. Notice the return value is signed integer.

#### Exercise 9

Just did every steps following the instructions.

Notice that the `kmalloc` returns virtual memory address (starts with a series of `0xfffff...`). So we have to use `virt_to_phys` macro to unwrap it before it could work normally.

### Grading

Finally, we restores the `-Werror` argument and `make grade`.

There's merely some implicit casting and unused variables cleaning work to do.

```
make grade
LAB3: test >>>>>>>>>>>>>>>>>
./scripts/grade-lab3
badinsn: (4.3s) 
  badinsn output: OK 
  badinsn2: (4.4s) 
  badinsn2 output: OK 
  hello: (4.3s) 
  hello output: OK 
  putc: (3.9s) 
  putc output: OK 
  createpmo: (4.2s) 
  createpmo output: OK 
  mappmo: (3.8s) 
  mappmo output: OK 
  mappmoerr: (3.9s) 
  mappmoerr output: OK 
  brk: (4.0s) 
  brk output: OK 
  faultread: (4.0s) 
  faultread output: OK 
  faultwrite: (4.1s) 
  faultwrite output: OK 
  pf: (4.1s) 
  pf output: OK 
Score: 100/100
```

See you in *Lab 4*.

