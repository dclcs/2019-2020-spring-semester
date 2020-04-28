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

