# ChCore Lab 1: Booting a machine

**Hand out: 2020-03-06**

**Deadline: 2020-03-25 23:59 (GMT+8) No Extension!**

**TA in charge: SHEN Sijie (ds_ssj \at sjtu.edu.cn )**

## Introduction

This lab is split into 3 parts. **Part A** concentrates on getting familiarized with ARM assembly language, the QEMU emulator, and the power-on bootstrap procedure. **Part B** examines the boot loader for ChCore. Finally, **Part C** delves into the initial template for our course kernel itself, named ChCore.

### Software Setup

You can build the lab environment as [tools](https://ipads.se.sjtu.edu.cn/courses/os/reference.shtml) under Linux. We also provide two types of virtual machines:

- VM for **VirtualBox** ([link](https://jbox.sjtu.edu.cn/l/xncuyo) password: os2020), you should install VirtualBox and import it
- VM for **VMware** ([link](https://jbox.sjtu.edu.cn/l/UHkDo1) password: os2020), you should install VMware player, VMware workstation or VMware Fusion.

In the both VM, the username:password pair is os:123.

In our ChCore labs, you need to use git to get and hand in your labs. If you are not familiar with git, don't worry. The [simple guide](https://rogerdudler.github.io/git-guide/) is very useful for newbies, and we also tell you some common usages of git in our lab.

You should read this [tutorial](https://ipads.se.sjtu.edu.cn/courses/os/lab/start.html) carefully and configure your git as this tutorial. You need *clone* your lab repository as following:

```
~$ cd chcore
chcore$ git fetch upstream
chcore$ git checkout -b lab1 upstream/lab1
chcore$ git push -u origin
```

### Grading and Hand-In Procedure

**Grading**: coding (80%) + document (20%)

For each ChCore lab, you need to hand-in a document to report how you do each exercise in this lab in detail. If you have some suggestions on ChCore lab, you can also write down in the document. In lab1, the document is named as `[StudentId]-lab1.pdf` or `[StudentId]-lab1.md` and stores under the `doc` directory (`mkdir doc` to create it). You can add it to your repo as:

```
chcore$ git add doc/[StudentId]-lab1.pdf
chcore$ git commit -m "add document"
chcore$ git push origin lab1
```

We encourage you usually commit your updates (both on code and document) and push them to our GitLab server, which we have told you in your *ICS* course.

```
chcore$ git commit -am "Exercise xx done!"
chcore$ git push origin lab1
```

To check your code grade, you can type:

```
chcore$ make grade
```

**Note**:

1. Except for documents, you can only modify the files we assign in each exercise. You cannot create any other files or modify scripts. We will overwrite them by origin files when grading. Each `git push`, make sure your lab can be built successfully; otherwise, you will get 0 points.
2. Except for the local test set, we also prepare a remote test set to test your code. The eventual score of your code is combined with both the local test set and the remote test set.

## Part A: Bootstrap

This part is aimed to introduce you to ARM assembly language and the bootstrap process, and to get you started with QEMU and QEMU/GDB debugging. You need to answer some questions **without writing any code**.

### Getting Started with AArch64 assembly

*AArch64* is the 64-bit execution state of the ARMv8 ISA. If you are not already familiar with ARM assembly language, you will quickly become familiar with it during this course! The [Arm® Instruction Set Reference Guide](https://static.docs.arm.com/100076/0100/arm_instruction_set_reference_guide_100076_0100_00_en.pdf) is an excellent place to start. In our ChCore labs, you do not need to write assembly in most cases, so you only need to be able to read it.

> **Exercise 1**. Read or at least carefully scan the **Part A1** and **A3** of the [Arm® Instruction Set Reference Guide](https://static.docs.arm.com/100076/0100/arm_instruction_set_reference_guide_100076_0100_00_en.pdf) to be familiar with ARM ISA. **Part D** is a useful reference for ARM instructions. You can scan the section 1 and 2 in [this guide](https://modexp.wordpress.com/2018/10/30/arm64-assembly/) to be familiar with gcc AArch64 grammar quickly. Please write down some differences compared with x86-64 you have learned before.

### Simulating AArch64 on Raspi3

Instead of developing the operating system on real hardware, we use a simulator, which emulates a complete Raspberry Pi 3 (*Raspi3*). It can simplify debugging, since some common debug methods can be used such as breakpoint. You can port it on real hardware if you are interested in it further.

We will use [QEMU](http://wiki.qemu.org/Index.html), a modern and relatively fast emulator. GDB can help us to debug the software in QEMU, which we will tell you how to use in this lab.

To get started, clone ChCore from your repository to `chcore` as described in "Software Setup", then use *docker* to build the minimal ChCore boot loader and kernel you will start with.

```
chcore$ make build
```

Now you're ready to run QEMU, supplying the file `build/kernel.img` created above, as the contents of the emulated Raspi3's "virtual flash". The entry of this flash image is designated by the link rule in `CMakefiles.txt`. It contains both our boot loader and our kernel.

```
chcore$ make qemu-nox
```

This executes QEMU with the options required to set the direct serial port output to the terminal. Some text should appear in the QEMU window:

```
[BOOT] Install boot page table
[BOOT] init boot page table: first_vaddr= first_paddr=
[BOOT] Enable el1 MMU
[BOOT] Jump to kernel main at 
```

To quit qemu, type `Ctrl+a x` (type `Ctrl` and `a` simultaneously, then type `x`).

### Debug with GDB

In this portion of the lab, you'll use QEMU's debugging facilities to investigate how the Raspi3 boots. Since we need GDB on an x86-64 (virtual) machine to debug the AArch64 code, `gdb-multiarch` is used, instead of the common `gdb`.

We need two terminal windows. In one, type

```
chcore$ make qemu-nox-gdb
```

This starts up QEMU, but QEMU stops just before the processor executes the first instruction and waits for a debugging connection from GDB.

In the second terminal, type

```
chcore$ make gdb
```

This command will start a gdb client listening on the default port (`tpc::1234`) of QEMU. We provide a gdb script `.gdbinit` to initialize the gdb. You should see something like this,

```
...
0x0000000000080000 in ?? ()
(gdb) 
```

> **Exercise 2**. Use GDB's `where` command to trace the entry (first function) and its address of the boot loader. Where is this function defined? You can also use `readelf -S build/kernel.img` to read the symbol table.

## PartB: The Boot Loader

Raspi3 boots from a `.img` file in a flash (SD card). The boot loader consists of one assembly language source file, `boot/start.S`, and one C source file, `boot/init_c.c` Look through these source files carefully and make sure you understand what's going on. The boot loader must perform two main functions:

1. First, the boot loader switches the processor into the el1 exception level by the function `arm64_elX_to_el1`. Exception levels are described briefly in the section A3.2 of [Arm® Instruction Set Reference Guide](https://static.docs.arm.com/100076/0100/arm_instruction_set_reference_guide_100076_0100_00_en.pdf).

   > **Exercise 3**. Before we introduce multi-processors, we focus on a single processor. In Respi3, all processors boot simultaneously. Please read the boot code in `boot/start.S` and explain how to hang other processors according to the control flow.

2. Second, the boot loader initializes boot UART, page table, and MMU. Then, the boot loader jumps to the real *kernel*. The memory structure is described in the following labs.

### Loading the Kernel

We will now look in further detail at the C language portion of the boot loader, in `boot/init_c.c`. But before doing so, it is a good time to stop and review the basics of C programming.

> **Exercise 4.** You have learned basic pointer operations in the ICS course. Then download the code for [pointer-warmup.c](https://ipads.se.sjtu.edu.cn/courses/os/lab/pointer-warmup.c), compile by `gcc pointer-warmup.c`, run `./a.out`, and make sure you understand all of the outputs. How all the values in printed lines 2 to 4 get there, and why the values printed in line 5 are seemingly corrupted.

*Warning:* Unless you are already thoroughly versed in C, do not skip or even skim this reading exercise. If you do not really understand pointers in C, you will suffer untold pain and misery in subsequent labs, and then eventually come to understand them in a hard way. Trust us; you don't want to find out what "the hard way" is.

To make sense of `boot/init_c.c`, you'll need to know what an ELF binary is. When you compile and link a C program such as the ChCore kernel, the compiler transforms each C source ('`.c`') file and assembly source file (`.S`) into an *object* ('`.o`') file containing assembly language instructions encoded in the binary format expected by the hardware. The linker then combines all of the compiled object files into a single *binary image* such as `build/kernel.img`, which in this case is a binary in the ELF format, which stands for "Executable and Linkable Format". The [Wikipedia page](http://en.wikipedia.org/wiki/Executable_and_Linkable_Format) has a short description.

For purposes of this lab, you can consider an ELF executable to be a header with loading information, followed by several *program sections*, each of which is a contiguous chunk of code or data intended to be loaded into memory at a specified address. The boot loader does not modify the code or data; it loads it into memory and starts executing it.

An ELF binary starts with a fixed-length *ELF header*, followed by a variable-length *program header* listing each of the program sections to be loaded. The program sections we're interested in are:

- `.text`: The program's executable instructions.
- `.rodata`: Read-only data, such as ASCII string constants produced by the C compiler. (We will not bother setting up the hardware to prohibit writing, however.)
- `.data`: The data section holds the program's initialized data, such as global variables declared with initializers like `int x = 5;`.
- `.bss`: The data section for *uninitialized* global variables, such as `int x;`. Since the uninitialized global variables are set as 0 during running, this section will not occupy real space in the ELF. The linker records just the address and size of the `.bss` section. The loader or the program itself must arrange to zero the `.bss` section.

Examine the full list of the names, sizes, and link addresses of all the sections in the kernel executable by typing:

```
chcore$ objdump -h build/kernel.img
```

You will see many more sections than the ones we listed above, but the others are not important for our purposes. Most of the others are to hold debugging information, which is typically included in the program's executable file but not loaded into memory by the program loader.

Take particular note of the "VMA" (or *link address*) and the "LMA" (or *load address*) of the `.text` section. The meaning of these two addresses are described in [this guide](https://www.zeuthen.desy.de/dv/documentation/unixguide/infohtml/binutils/docs/ld/Basic-Script-Concepts.html#Basic-Script-Concepts): The first is the VMA, or virtual memory address. This is the address the section will have when the output file is running. The second is the LMA, or load memory address. This is the address at which the section will be loaded. In most cases, the two addresses will be the same. An example of when they might be different is when a data section is loaded into ROM, and then copied into RAM when the program starts up (this technique is often used to initialize global variables in a ROM based system). In this case, the ROM address would be the LMA, and the RAM address would be the VMA.

> **Exercise 5**. Read the `objdump` information of `build/kernel.img`, the code of boot loader is stored in `.init` section, and codes of the kernel are stored in `.text`, explain the differences of their LMA and VMA. And try to explain why these differences happened.

## Part C: The Kernel

### Formatted Printing to the Console

Most people take functions like `printf()` for granted, sometimes even thinking of them as "primitives" of the C language. But in an OS kernel, we have to implement all I/O ourselves. In ChCore, there are two kinds of `printf` for the boot loader and kernel with the same function, defined in `boot/print/printf.c` and `kernel/common/printk.c`, respectively. Make sure you understand the relationship of functions defined in these two files.

> **Exercise 6**. `tfp_printf` and `printk` are the interface function, and they support format print similar to standard `printf`. While we omit the code to print numbers in different bases (e.g., 2, 4, 8, 16), please fill the functions `write_num` in `boot/print/printf.c` and `printk_write_num` in `kernel/common/printk.c` to implement this function. They have the same logic.

After doing this exercise correctly, you can type `make grade`, and you can pass the `boot print` test and the `kernel print` test.

### Stack

In the final exercise of this lab, we will explore in more detail how C language uses the stack on the AArch64. You need to write a useful new kernel monitor function. This function prints a *backtrace* of the stack: a list of the saved Program Counter (PC) values from the nested `bl` instructions (similar to `call` in x86) that led to the current point of execution.

> **Exercise 7.** Where do codes of kernel stack initialization lay? Which function does the kernel initialize its stack? Where is its stack located in the memory? How does the kernel reserve space for its stack?

Don't worry about the stack in AArch64. The calling convention is similar to x86-64, which we have studied in our ICS course.

The AArch64 stack pointer (`sp` register) points to the lowest location on the stack that is currently in use. Everything *below* that location in the region reserved for the stack is free. Pushing a value onto the stack involves decreasing the stack pointer and then writing the value to the place the stack pointer points to. Popping a value from the stack involves reading the value the stack pointer points to and then increasing the stack pointer. In AArch64, the stack holds 64-bit values, and `sp` is always divisible by 8. Various AArch64 instructions, such as `bl`, are "hard-wired" to use the stack pointer register.

The `x29` (aka. `fp`, frame pointer) register, in contrast, is associated with the stack primarily by software convention. On entry to a C function, the function's *prologue* code normally saves the previous function's base pointer by pushing it onto the stack, and then copies the current `sp` value into `fp` for the duration of the function.

According to this convention, it is possible to trace back through the stack by following the chain of saved `fp` pointers and determining the nested sequence of function calls. This capability can be particularly useful, for example, when a particular function causes an `assert` failure or `panic` because bad arguments were passed to it, but you aren't sure *who* passed the bad arguments. A stack backtrace lets you find the offending function.

In gdb, you can print a register as address and its 64-bit value or arrays like:

```
(gdb) x/g $x29                                                                                          
0xffffff000020f330 <kernel_stack+7984>: 0xffffff000020f350
(gdb) x/10g $x29
0xffffff000020f330 <kernel_stack+7984>: 0xffffff000020f350      0xffffff00000d009c
0xffffff000020f340 <kernel_stack+8000>: 0x0000000000000001      0x000000000000003e
0xffffff000020f350 <kernel_stack+8016>: 0xffffff000020f370      0xffffff00000d009c
0xffffff000020f360 <kernel_stack+8032>: 0x0000000000000002      0x000000000000003e
0xffffff000020f370 <kernel_stack+8048>: 0xffffff000020f390      0xffffff00000d009c
```

> **Exercise 8.** To become familiar with the C calling conventions on the AArch64, find the address of the `test_backtrace` function in `kernel/main.c` by `gdb`, set a breakpoint there, and examine what happens each time it gets called after the kernel starts. How many 64-bit words does each recursive nesting level of `test_backtrace` push on the stack, and what are those words?

To understand what a function call happens in the assembly level in AArch64, you can use `gdb` to read the assembly codes by running the first instruction (`s`) and typing `x/30i test_backtrace` command to read 30 lines of assembly codes in`test_backtrace` function. You should be able to answer the following question.

> **Exercise 9.** In AArch64, return address (`x30`), frame pointer (`x29`), and arguments are transmitted by registers. However, when a caller function calls a callee function, these registers should be stored in the stack, where do these values store? This [reference](https://thinkingeek.com/2017/05/29/exploring-aarch64-assembler-chapter-8/) will help you to answer better.

The above exercise should give you the information you need to implement a stack backtrace function, which you should call `mon_backtrace()`. A prototype for this function is already waiting for you in `kernel/monitor.c`. You can do it entirely in C, but you may find the `read_fp()` function in this file useful. For AArch64 gcc inline assembler, you can refer [ARM GCC Inline Assembler Cookbook](http://www.ethernut.de/en/documents/arm-inline-asm.html).

The backtrace function should display a listing of function call frames in the following format:

```
Stack backtrace:
  LR ffffff00000d009c  FP ffffff000020f330  Args 0 0 ffffff000020f350 ffffff00000d009c 1
  LR ffffff00000d009c  FP ffffff000020f350  Args 1 3e ffffff000020f370 ffffff00000d009c 2
  LR ffffff00000d009c  FP ffffff000020f370  Args 2 3e ffffff000020f390 ffffff00000d009c 3
  ...

```

Each line contains an `LR`, `FP`, and `Args`. The `FP` value indicates the frame pointer (`x29`) into the stack used by that function: i.e., the position of the stack pointer just after the function was entered and the function prologue code set up the frame pointer. The listed `LR` value is the function's *link register* (aka. `x30`), which stores *return instruction pointer*: the instruction address to which control will return when the function returns. The return instruction pointer typically points to the instruction after the `bl` instruction (why?). The `bl [label]` instruction jumps to a label and sets the register `x30` to `PC+4`. It provides a hint that this is a subroutine call. Finally, the five hex values listed after `Args` are the first five arguments to the function in question, which would have been pushed on the stack just before the function was called. If the function was called with fewer than five arguments, of course, then not all five of these values will be useful.

**We ignore `mon_backtrace` itself**. The first line printed reflects the function that called `mon_backtrace`, the second line reflects the function that called that one, and so on. You should print *all* the outstanding stack frames. By studying `boot/start.S` you'll find that there is an easy way to tell when to stop.

Here are a few specific points that are worth remembering for the following exercise and for future labs.

- If `long *p = (long *) 100`, then `(long)p + 1` and `(long) (p + 1)` are different numbers: the first is `101` but the second is `108`. When adding an integer to a pointer, as in the second case, the integer is implicitly multiplied by the size of the object the pointer points to.
- `p[i]` is defined to be the same as `*(p+i)`, referring to the i'th object in the memory pointed to by `p`. The above rule for addition helps this definition work when the objects are larger than one byte.
- `&p[i]` is the same as `(p+i)`, yielding the address of the i'th object in the memory pointed to by `p`.

Although most C programs never need to cast between pointers and integers, operating systems frequently do. Whenever you see an addition involving a memory address, ask yourself whether it is an integer addition or pointer addition and make sure the value being added is appropriately multiplied or not.

> **Exercise 10.** Implement the backtrace function `mon_monitor` in `kernel/monitor.c` as specified above. Use the same format as in the example, since otherwise the grading script will be confused.

If you use `read_ebp()`, note that GCC may generate "optimized" codes that call `read_ebp()` *before* `mon_backtrace()`'s function prologue, which results in an incomplete stack trace (the stack frame of the most recent function call is missing). While we have tried to disable optimizations that cause this reordering, you may want to examine the assembly of `mon_backtrace()` and make sure the call to `read_ebp()` is happening after the function prologue.

After completing Exercise 11 correctly, you can get all of the 80 points by typing `make grade`.

```
LAB1: test >>>>>>>>>>>>>>>>>
./scripts/grade-lab1
running ChCore:
  boot print: OK
  kernel print: OK
  backtrace count: OK
  backtrace arguments: OK
  backtrace lr: OK
Score: 80/80
```

### Thanks and congratulation!