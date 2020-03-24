# SE-315

## Lab 1 Docs

### Environment

Provided VMware virtual machine was used, based on VMware Fusion Pro (`v11`).

Files were tracked under `git` version control system.

> Glitch:
>
> `./scripts/docker_build.sh` didn't handle the situation that `$(pwd)` may contain space or other special characters.
>
> I used `VMware Tools` to mount virtual volumes to `os-labs-vm`, and happened to stuck in that minor issue.
>
> The solution is simple: using quotation marks to wrap the `$(pwd)` should fix that.
>
> `docker run -it <...> -v "$(pwd)":/chos <...>`

### Exercises

#### Exercise 1

Differences between x86-64...

##### Registers

* More Registers.
	* `ARMv8`, as a *CISC* ISA, has more than 30 general purpose registers, while `x86-64` has only 16 visible registers (and only 8 of them can be used for general purposes).
* Isolated Registers for Exception Levels.
	* There are 4 different privilege levels in `ARMv8` Architecture. Hardware registers are provided separately to ensure the isolation.
	* However, `x86-64` has no such hardware isolation design.
* Saved Program Status Registers Manipulating Methods.
	* Some critical runtime states are saved as grouped bits in a special register called SPSR. Using `LDR` or `STR` can directly manipulate that register.
	* `x86-64` has a counterpart called `EFLAGS`, however it can't be simply read or written by general instructions, but with special instructions like `LAHF` and `SAHF`.

##### Privilege Levels

* The Greater, the Higher.
	* In `ARMv8`, there are 4 exception levels. The greater the level number is, the higher privilege it could obtain.
		* Generally, the OS kernel **doesn't** run in the highest privilege level `EL3`, but in `EL1`, just one level higher than user-modal applications.
		* `EL2` and `EL3` is reserved by hardware monitor codes which is strictly verified flawlessly.
	* However, the *Protection Ring* System as the counterpart in `x86-64` works differently.  The lesser the level number is, the higher privilege it could obtain.
		* Generally, the OS kernel runs in the highest privilege level *Ring 0*.
		* There's no higher-privileged system working beyond the OS kernel. So any bugs lies in OS kernel could be fatal.

##### Instructions

* Instruction Lengths: Static or Dynamic.
	* `ARMv8`'s instructions have all the same length, which is 32 bits, or 4 Bytes.
		* That means *PC* (Program Counter)'s will always increase itself by 4 (Bytes) after each instruction fetching.
	* However, `x86-64` 's instruction length isn't static. That could reduce the binary code size but complicate the integrated-circuit implementation.
* Different Parallel Computing Implementation.
	* In `ARMv8`, spacial parallel computation is implemented via *aSIMD* technique.
	* However in `x86-64`, it was implemented by *SSE* technique.

* Conditional Instructions.
	* In `ARMv8`, there are very few conditional instructions left.
		* The only one is the `B.cond` instruction, which judges the `cond` condition and decide if it should perform a branch jump.
	* However in `x86-64`, there are much more conditional instructions like `cmov` (conditional moving).
* Updating Condition Bits.
	* In `x86-64`, all arithmetic instructions will automatically update the condition bits.
	* However in `ARMv8`, those normal arithmetic instructions won't update those bits.
		* Only those with `S` prefix will set the condition bits.

#### Exercise 2

The very first function running in the kernel is `_start`, which was defined in `./boot/start.S`, in line 12...

```
(gdb) where
#0  0x0000000000080000 in _start ()
```

...and its address appears to be `0x0000000000080000`.

By analyzing the kernel image `./build/kernel.img`, here's what i've got:

```
Symbol table '.symtab' contains 119 entries:
   Num:    Value          Size Type    Bind   Vis      Ndx Name
     0: 0000000000000000     0 NOTYPE  LOCAL  DEFAULT  UND 
     1: 0000000000080000     0 SECTION LOCAL  DEFAULT    1 
     2: ffffff00000cc000     0 SECTION LOCAL  DEFAULT    2 
	... ...
    91: 0000000000080000    48 FUNC    GLOBAL DEFAULT    1 _start
	... ...
   117: 0000000000086000  4096 OBJECT  GLOBAL DEFAULT    1 _boot_pgd_down
   118: 0000000000087210   160 FUNC    GLOBAL DEFAULT    1 el1_mmu_activate

```

Symbol #91 is the first function mentioned above.

> Document Typo:
>
> Option `-S` (capital) or `--sections` actually prints the section headers rather than the symbol table.
>
> Option `-s` (non-capital) or `--symbols` should print the symbol table well.

#### Exercise 3

Critical codes here:

```assembly
BEGIN_FUNC(_start)
  /* get the processor id, and store it in register x8 */
	mrs	x8, mpidr_el1
	mov	x9, #0xc1000000
	bic	x8, x8, x9
	cbz	x8, primary

  /* hang all secondary processors before we introduce multi-processors */
secondary_hang:
	bl secondary_hang
```

##### Dead-loop

Obviously, the `secondary_hang` branch is a dead-loop. Any control-flow goes there will stall forever.

So, in order to block all secondary processors, we just need to guide their control flow into label `secondary_hang`.

##### Distinguish Processors

How can we distinguish the main processor from other secondary processors?

A special register called `mpidr_el1` differs among Raspberry Pi 3's processors.

According to ARM's [documentation](http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.101111_0101_04_en/lau1443447573920.html), the register `mpidr_el1` contains many useful informations, as shown below:

![image-20200322190002598](report.assets/image-20200322190002598.png)

`MPIDR_EL1[30]`, also named `U`, indicates if current core is part of a multiprocessor system. If so, `U` should be `0`. Or else, it should be `1`.

`MPIDR_EL1[24]`, also named `MT`, indicates whether the lowest level of affinity consists of logical cores that are implemented using a multithreading type approach. If so, `MT` should be `1`. Or else, it should be `0`.

Let's think. If our system has only one processor core, we should never stall the current (only!) processor. Or, if our system is a multiprocessor one, we should stall all those secondary cores and only keep the first core running.

So in a word, if a core's `U` and `MT` are all `0`, we should stall this core because **a)** it is not the only core in the system and **b)** it's not the primary core. So the mask code should be `0100 0001 0000 0000 0000 0000 0000 0000`, or `0x41000000` in hex.

> In `./boot/start.S` it uses `0xc1000000`, which includes the bit `MPIDR_EL1[31]`, which is a reserved bit. Can't figure out why.

If it goes through the mask (by `bic x8, x8, x9`) and turns out to be zero, that means the current core shouldn't be stall. So, we jump to a normal branch `primary` to continue the initialization.

#### Exercise 4

##### Part 0

```c
long a[4];
long *b = malloc(16);
long *c;
```

`a` is an array containing 4 `long` integers. Since it's a fixed-length array, it would be allocated on current stack frame.

`b` is a pointer to `long` integer, which points to a $16$ Bytes-long memory space, whereas `sizeof(long)` is $8$ Bytes in my testing environment. It's allocated dynamically at runtime, so it would points to a heap memory address.

`c` is a pointer to `long` integer, which wasn't initialized yet.

##### Part 1

```c
  printf("1: a = %p, b = %p, c = %p\n", a, b, c);
  c = a;
```

The output is shown as following:

```
1: a = 0x7fff56b2ca00, b = 0x7f9824c026a0, c = 0x3000000008
```

As described above, `0x7fff56b2ca00` is a stack memory address, while `0x7f9824c026a0` is a heap memory address. As for `0x3000000008`, it's totally a meaningless garbage value, and memory address `0x3000000008` is inaccessible.

Trying to dereference memory address ``0x3000000008`` will cause a segmentation fault.

##### Part 2

```c
	// 2. What's the mean of `c = a`?
	for (int i = 0; i < 4; i++)
		a[i] = 100 + i;
	c[0] = 200;
	printf("2: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n",
		   a[0], a[1], a[2], a[3]);
```

`c = a` means now pointer `long *c` will now points to the first element in `long` array `a[4]`.

So, in the `for` loop, we put array `[100, 101, 102, 103]` into `long a[4]`. After that, we use `c` (which is an alias of `a` now) to modify the first element in array `a[4]` as `200`.

So, now the array holds values `[200, 101, 102, 103]`.

##### Part 3

```c
	// 3. the usage of []
	c[1] = 300;
	*(c + 2) = 301;
	3 [c] = 302;
	printf("3: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n",
		   a[0], a[1], a[2], a[3]);
```

As mentioned above, `c` points to the first element in array `a`. So those codes still manipulates array `a` via pointer `c`.

Notice that `a[b]` is somehow a syntactic sugar, which is an alias of `*(a + b)`. And in C/C++ if we perform addition operation between a pointer `p` and an integer `i`, the result would be always `p + i * sizeof(*p)`.

Due to that reason, `3[c]` is no different from `c[3]` at all.

##### Part 4

```c
	// 4. the meaning pointer add
	c = c + 1;
	*c = 400;
	printf("4: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n",
		   a[0], a[1], a[2], a[3]);
```

Already mentioned above, addition between a pointer and an integer will be interpreted as `p + i * sizeof(*p)`. So `c = c + 1` or `++c` will make pointer `c` to `a[1]` now.

After the statement `*c = 400`, `a[1]` will be `400` now.

##### Part 5

```c
	// 5. the meaning pointer add
	c = (long *)((char *)c + 1);
	*c = 500;
	printf("5: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n",
		   a[0], a[1], a[2], a[3]);
```

Now, before `c` was incremented by 1, it was `reinterpreter_cast`ed to `char *`. Since `sizeof(char)` is 1, doing the increment will only add 1 Byte offset in memory.

$500$ in binary is `.... (leading 0s) 0000 0001 1111 0100`. Before Part 5, the array `a` should look like this:

```
a[0]: 200
a[1]: 400
a[2]: 300
a[3]: 302
```

However, in a little-endian machine, `a[1:2]` memory layout is:

```
1001 0000 0000 0001 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
----
0010 1100 0000 0001 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
```

And our literal number 500 with 1-Byte offset is:

```
.... .... 1111 0100 0000 0001 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
----
0000 0000 .... .... .... .... .... .... .... .... .... .... .... .... .... ....
```

After that manipulation, `a[1:2]` memory layout would be:

```
1001 0000 1111 0100 0000 0001 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
----
0000 0000 0000 0001 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000 0000
```

Converted into decimal, they would be `128144` and `256`.

##### Part 6

```c
	// 6. the meaning pointer add
	b = (long *)a + 1;
	c = (long *)((char *)a + 1);
	printf("6: a = %p, b = %p, c = %p\n", a, b, c);
```

Same story as Part 5. Notice that in line `b = (long *)a + 1`, the cast operation has higher priority than `+`, so it would convert `a` into `long *` first, increase it by 1 unit (or 8 Bytes here) then.

#### Exercise 5

By  `objdump -h ./build/kernel.img`, we can get the following output:

```
./build/kernel.img:     file format elf64-little

Sections:
Idx Name          Size      VMA               LMA               File off  Algn
  0 init          00049680  0000000000080000  0000000000080000  00010000  2**12
                  CONTENTS, ALLOC, LOAD, CODE
  1 .text         00000760  ffffff00000cc000  00000000000cc000  0005c000  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, CODE
  2 .data         00000000  ffffff00000d0000  00000000000d0000  00060000  2**0
                  CONTENTS, ALLOC, LOAD, DATA
  3 .rodata       000000d0  ffffff00000d0000  00000000000d0000  00060000  2**3
                  CONTENTS, ALLOC, LOAD, READONLY, DATA
  4 .bss          00008000  ffffff00000d00d0  00000000000d00d0  000600d0  2**4
                  ALLOC
< ... other sections ... >
```

Easy to tell that Bootloader codes has the same `VMA` as `LMA`, but OS kernel codes' `VMA = ffffff0000000000 | LMA`, whose 24 higher bits are set to 1 exclusively.

Bootloader codes are usually solidified in flash chips, not in ROM. So there's no need and no way to map them into memory, because those codes can be directly referenced by CPU data bus. Obviously its `VMA` and `LMA` should be the same.

However, OS kernel's code was stored in disk or ROM. Those codes must be mapped into a memory space before being executed. In normal computer architectures, OS kernel takes the higher end of the virtual memory space, that's why its address begins with `0xffffff`.

#### Exercise 6

The main function of `write_num` or `printk_write_num` is basically convert an `unsigned long n` into any given base.

We can always get the least significant digit by `n % base`, and disregard that digit by `n /= base`. Repeat till `n` is 0.

However, we get the least significant digit first and the most significant digit last in such way. So reversing the string before calling `write_string` is necessary.

In order to verify its correctness, I wrote `./report/src/test.py` to execute random tests.

#### Exercise 7

OS kernel stack allocation code lies in `./kernel/main.c`. It is merely a size-fixed memory buffer, one for each kernel.

```c
char kernel_stack[PLAT_CPU_NUM][KERNEL_STACK_SIZE];
```

However, that memory buffer won't be a stack till the code in `./kernel/head.S` is executed.

Function `start_kernel` in `./kernel/head.S` puts the pointer to the **tail** of `kernel_stack` buffer to register `sp` (aka. `x29`, or stack pointer). Since then, `kernel_stack` buffer begins to play its kernel stack role.

`kernel_stack` is declared as a global array in `./kernel/main.c`, which is allocated in kernel's heap, which lies in kernel's high privilege address space. This address space is inaccessible for user-modal codes.

#### Exercise 8

After `make qemu-nox-gdb` and `make gdb`, we can enter the gdb debugging mode.

By executing `b test_backtrace` to set a breakpoint at that function, GDB points out that

```
Breakpoint 1 at 0xffffff00000cc020: file ../kernel/main.c, line 26.
```

So we can know that the address of function `test_backtrace` lies in memory address `0xffffff00000cc020`.

After the program hits that breakpoint, it's about to execute the following instructions:

```assembly
; B+> │ 0xffffff00000cc020 <test_backtrace>

<test_backtrace>		stp    x29, x30, [sp, #-32]!
						; saves x29 and x30 into stack
						
						mov    x29, sp
						; x29 <= stack pointer
						
						str    x19, [sp, #16]
						; expands the stack
						
						mov    x19, x0
						; x19 <= x0
						
						mov    x1, x0
						; x1 <= x0
						
						adrp   x0, 0xffffff00000d0000
						; x0 <= 0xffffff00000d0000's page table id
						
						add    x0, x0, #0x0
						; test x0
						
						bl     0xffffff00000cc8a4 <printk>
						; call printk
						
						cmp    x19, #0x0
						; compare x19 and 0
						
						b.gt   0xffffff00000cc068 <test_backtrace+72>
						; if x19 > 0, go to test_backtrace+72
						
						bl     0xffffff00000cc0b0 <mon_backtrace>
						; else, call mon_backtrace
						
<test_backtrace+44>		mov    x1, x19
						; x1 <= x19
						
						adrp   x0, 0xffffff00000d0000
						; x0 <= 0xffffff00000d0000's page table id
						
						add    x0, x0, #0x28
						; x0 <= x0 + 0x28
						
						bl     0xffffff00000cc8a4 <printk>
						; call printk
						
						ldr    x19, [sp, #16]
						; shrink the stack
						
						ldp    x29, x30, [sp], #32
						; resume x29 and x30 from stack
						
						ret
						; return from test_backtrace
						
<test_backtrace+72>		sub    x0, x19, #0x1
						; x0 <= x19 - 1
						
						bl     0xffffff00000cc020 <test_backtrace>
						; call test_backtrace
						
						b      0xffffff00000cc04c <test_backtrace+44> 
						; jump to test_backtrace+44
```

By analyzing them, we can know that function `test_backtrace` is recursively defined.

First program's control flow goes from`<test_backtrace>` to `<test_backtrace+40>`, saving registers, loading necessary values from stack.

Then, by detecting `x0`'s value, which contains the first function argument `test_backtrace` was called, the function will flows to `<test_backtrace+72>` or `<test_backtrace+44>`.

`<test_backtrace+44>` to `<test_backtrace+68>`  did some sweeping jobs, and returns from `test_backtrace` after then.

`<test_backtrace+72>` recursively calls itself, but its first argument `x0` is reduced by 1.

Every time when `test_backtrace` is called, it will expand the stack by 16 Bytes, saving registers `x29` and `x30` into stack to avoid it being corrupted.

They must be well-saved because `x29` (aka. `fp`) stores frame pointer, while `x30` (aka. `lr`) stores function return address.

#### Exercise 9

As mentioned above in Exercise 8, it is proved that every time when a function gets called, it will firstly saves its `x29` and `x30` before making any further function call. And when it's about to return from its lifetime, it will recover `x29` and `x30` from its stack.

> Actually, in order to accelerate such paradigm-like operations, ARMv8 provides `stp` and `ldp` to allow you load / store 2 registers from/to stack at the same time.

Why that should works? Well, because the only time a function requires `x29` and `x30` is its returning time. It needs `x29` to recover the stack frame correctly and needs `x30` to know where to return to. During the whole executing process, they are all not necessary. Notice that we can use frame pointer rather than stack pointer to access the stack.

#### Exercise 10

