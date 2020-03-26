 

# Chcore Lab 2: Memory Management

**Hand out: 2020-3-20 Deadline: 2020-4-15** **TA in charge: Feng Erhu ([fengerhu1@sjtu.edu.cn](https://ipads.se.sjtu.edu.cn/courses/os/lab/mailto:fengerhu1@sjtu.edu.cn) )**

**lab last modified time: 2020-3-23 19:15**

## Introduction

In this lab, you will write the memory management code for your operating system. Memory management has two components.

The first component is a physical memory allocator for the kernel, so that the kernel can allocate memory and later free it. Your allocator will operate in units of 4096 bytes, called *pages*. Your task will be to maintain data structures that record which physical pages are free and which are allocated, and how many processes are sharing each allocated page. You will also write the routines to allocate and free pages of memory.

The second component of memory management is *virtual memory*, which maps the virtual addresses used by kernel and user software to addresses in physical memory. The ARM64 hardware's memory management unit (MMU) performs the mapping when instructions use memory, consulting a set of page tables. You will modify Chcore to set up the MMU's page tables according to a specification we provide.

### Getting started

In this and future labs you will progressively build up your kernel. We will also provide you with some additional source. To fetch that source, use Git to commit your lab 1 source, fetch the latest version of the course repository, and then create a local branch called `lab2` based on our lab2 branch, `upstream/lab2`:

```
# fetch the remote updates, you are in branch lab1
chcore$ git fetch upstream 

# you switch to the branch lab2, whose code is based on the empty code provided by the TAs
chcore$ git checkout -b lab2 upstream/lab2 
Branch 'lab2' set up to track remote branch 'lab2' from 'upstream'.
Switched to a new branch 'lab2'

# update the remote tracking branch to your origin repo instead of the upstream repo
chcore$ git push -u origin    
To https://ipads.se.sjtu.edu.cn:2020/[username]/chcore.git
 * [new branch]      lab1 -> lab1
Branch 'lab1' set up to track remote branch 'lab1' from 'origin'.
```

The `git checkout -b` command shown above actually does two things: it first creates a local branch `lab2` that is based on the `upstream/lab2` branch provided by the course TA, and second, it changes the contents of your `lab` directory to reflect the files stored on the `lab2` branch. Git allows switching between existing branches using git checkout *branch-name*, though you should commit any outstanding changes on one branch before switching to a different one.

You will now need to merge the changes you made in your `lab1` branch into the `lab2` branch, as follows:

```
chcore$ git merge lab1
Auto-merging kernel/main.c
Auto-merging Makefile
Auto-merging CMakeLists.txt
Merge made by the 'recursive' strategy.
 Merge made by the 'recursive' strategy.
 boot/print/printf.c    | 32 ++++++++++++++++++++++++++++++--
 kernel/common/printk.c | 34 +++++++++++++++++++++++++++++++---
 kernel/monitor.c       | 12 ++++++++++++
 3 files changed, 73 insertions(+), 5 deletions(-)
```

In some cases, Git may not be able to figure out how to merge your changes with the new lab assignment (e.g. if you modified some of the code that is changed in the second lab assignment). In that case, the git merge command will tell you which files are *conflicted*, and you should first resolve the conflict (by editing the relevant files) and then commit the resulting files with git commit -a.

Lab 2 contains the following new source files, which you should browse through:

- `kernel/buddy_struct.h`
- `kernel/buddy.c`
- `kernel/buddy.h`
- `kernel/mm.c`
- `kernel/page_table.c`
- `kernel/page_table.h`
- `kernel/page_table.S`
- ...

`buddy_struct.h` describes some useful structures to organize the physical memory. Page structure that you'll use to keep track of which pages of physical memory are free. The code in `buddy.c` implements a sample buddy system (**review** you have already learned buddy system in ics ). `page_table.c`  manipulates virtual address mapping to corresponding physical address with legal permission bits.

Pay particular attention to `buddy_struct.h` and `page_table.h`, since this lab requires you to use and understand many of the definitions they contain.

### Lab Requirements

In this lab and subsequent labs, do all of the regular exercises described in the lab and *at least one* challenge problem. (Some challenge problems are more challenging than others, of course!) Additionally, write up brief answers to the questions posed in the lab and a short (e.g., one or two paragraph) description of what you did to solve your chosen challenge problem. If you implement more than one challenge problem, you only need to describe one of them in the write-up, though of course you are welcome to do more. Place the write-up in a file called `answers-lab2.txt` in the top level of your `lab` directory before handing in your work.

### Hand-In Procedure

When you are ready to hand in your lab code and write-up, run make handin in the `lab` directory. This will first do a make clean to clean out any `.o` files and executables, and then create a tar file called `lab2-handin.tar.gz` with the entire contents of your lab directory.

As described in chcore lab1, you need to hand in your source code and a document to describe the design of lab2(**The more detail, the more credits you may get!**).The hand-in your source code using git command:

```
git add .
git commit -m"finish lab2"
git push origin lab2
```

As before, we will be grading your solutions with a grading program. You can run make grade in the `lab` directory to test your kernel with the grading program. You may change any of the kernel source and header files you need to in order to complete the lab, but needless to say you must not change or otherwise subvert the grading code.

```
Erhu Feng is the TA responsible for this lab, please contact him for any questions. 
Email: fengerhu1@sjtu.edu.cn
```

## Part 1: Physical Page Management

The operating system must keep track of which parts of physical RAM are free and which are currently in use. Chcore manages the PC's physical memory with *page granularity* so that it can use the MMU to map and protect each piece of allocated memory.

The following figure shows physical memory layout for Chcore:

```
+------------------+  <- page_end (metadata_end + (npages * PAGE_SIZE))
|                  |
|                  |
|                  |
|      page        |
|                  |
|                  |  
|                  |
+------------------+  <- metadata_end (img_end +(npages * sizeof(struct page))) 
|     metadata     |
|                  |
+------------------+  <- metadata_start (img_end)
|    KERNEL IMG    |
+------------------+  
|  BOOT CODE&STACK |
+------------------+  <- 0x00008000 (img_start)
|     reserved     |
+------------------+  <- 0x00000000
```

The physical address [0x00000000 : 0x00008000] is reserved. The value of `img_start` is hardcode at the compilation time. The first part in [img_start : img_end] is boot code and cpu boot stack, each cpu stack is 4K size. Kernel image is stored in the upper part in physical address [img_start : img_end] and contains the code and data/metadata for kernel. Upper to the kernel image, the free physical memory is organized by physical page allocator. Allocator will divide the memory zone into two ranges: metadata range and page range. In the metadata range, it records page metadata like `list_node` and `flags` etc.. In the page range, it stores the real data.	 

> **Question** :
>
> Make a brief explanation: In which file or code segment specifies the CHcore physical memory layout. You can answer this question in two aspects: the compilation phase and runtime phase?

You'll now write the physical page allocator. It keeps track of which pages are free with a linked list of `struct Page` objects, each corresponding to a physical page. You need to write the physical page allocator before you can write the rest of the virtual memory implementation, because your page table management code will need to allocate physical memory in which to store page tables.

In Chcore, it implements a simple buddy system to organize the physical pages. Review what you have learned in ics. Every memory block in this system has an *order*, where the order is an integer ranging from 0 to a specified upper limit`BUDDY_MAX_ORDER`. The size of a block of order n is proportional to 2**n, so that the blocks are exactly twice the size of blocks that are one order lower. Power-of-two block sizes make address computation simple, because all buddies are aligned on memory address boundaries that are powers of two. When a larger block is split, it is divided into two smaller blocks, and each smaller block becomes a unique buddy to the other. A split block can only be merged with its unique buddy block, which then reforms the larger block they were split from

 `struct global_mem` holds the metadata `struct free_list` for each free list in the buddy system and `struct list_head` links buddy blocks. There are some useful functions to manipulate the `struct list_head`: 

- init_list_head(struct list_head * head ): Initialize the list head
- list_add(struct list_head *new, struct list_head *head): Add new node into the head of list
- list_del(struct list_head *entry): delete this node in list
- list_entry(ptr, type, member): Get the corresponding object using a given ptr. Member is the variable name of struct list_head in the corresponding object

> **Exercise 1.** In the file `kernel/mm/buddy.c`, you must implement code for the following functions.
>
> ```
> buddy_get_pages()
> __alloc_page()
> buddy_free_pages()
> ```

You can invoke the assisted functions: `get_buddy_page()`,`get_merge_page()`,`split()` which are already implemented. Read the annotations carefully before using these functions.

 `test_buddy()` test your physical page allocator. You should make the test files under the `/test/mm/buddy` using the following scripts:

```
make docker
cd ./test/mm/buddy
cmake ./
make
./test_buddy
```

If you pass the all tests the expected output:

```
1 tests, 640510 assertions, 0 failures
```

Run test_buddy and see whether reports 0 failures. Fix your code so that it passes. You may find it helpful to add your own `mu_check()`s to verify that your assumptions are correct.

This lab, and all the labs, will require you to do a bit of detective work to figure out exactly what you need to do. This assignment does not describe all the details of the code you'll have to add to Chcore. Look for comments in the parts of the Chcore source that you have to modify; those comments often contain specifications and hints.

## Part 2: Virtual Memory

Before doing anything else, familiarize yourself with the arm64's protected-mode memory management architecture: namely *page translation*.

> Hint:  Look at chapters D5 of the [Arm® Architecture Reference Manual](https://static.docs.arm.com/ddi0487/fa/DDI0487F_a_armv8_arm.pdf?_ga=2.181644388.2107974726.1583153879-1487747685.1581514464) (AArch64 Virtual Memory System Architecture), if you haven't done so already, you should read the following materials carefully. However, if you are proficient in arm64 address translation and arm-smmu architecture , you can do the exercise 2 directly

**ARM64 Address translation**

ARM64 uses a Virtual Memory system where the addresses used by code (virtual addresses) are translated into physical addresses which are used by the memory system. This translation is performed by a part of the processor that is called a Memory Management Unit (MMU). MMUs in the ARM architecture use translation tables stored in memory to translate virtual addresses to physical addresses. The MMU will automatically read the translation tables when necessary, this process is known as a Table Walk

**Separation of kernel and application virtual address spaces**

Operating systems typically have several applications or tasks running concurrently. Each of these has its own unique set of translation tables and the kernel switches from one to another as part of the process of context switching between one task and another. However, much of the memory system is used only by the kernel and has fixed virtual to physical address mappings where the translation table entries rarely change. The ARM64 architecture provides several features to efficiently handle this requirement

The table base addresses are specified in the Translation Table Base Registers (**TTBR0_EL1**) and (**TTBR1_EL1**). The translation table pointed to by TTBR0 is selected when the upper bits of the virtual address (VA) are all set to 0. TTBR1 is selected when the upper bits of the VA are all set to 1. You can enable VA tagging to exclude the top 8 bits from the check![ttbr](../../../../../../../.\image\ttbr.png)

**Translating a virtual address to a physical address**

When the processor issues a virtual address for an instruction fetch, or data access, the MMU hardware translates the virtual address to the corresponding physical address. For a virtual address in an n -bit address space, the top 64-n bits VA[63:n] must be all 0s or 1s, otherwise the address triggers a fault. 

The least significant bits are then used to give an offset within the selected section, so that the MMU combines the physical address bits from the block table entry with the least significant bits from the original address to produce the final address.

In the case of a 4kB granule, the hardware can use a 4-level look up process. The 48-bit address has nine address bits for each level translated (that is, 512 entries each), with the final 12 bits selecting a byte within the 4kB coming directly from the original address. 

Bits [47:39] of the virtual address index into the 512 entry L0 table. Each of these table entries spans a 512GB range and points to an L1 table. Within that 512 entry L1 table, bits [38:30] are used as index to select an entry and each entry points to either a 1GB block or an L2 table. 

Bits [29:21] index into a 512 entry L2 table and each entry points to a 2MB block or next table level. At the last level, bits [20:12] index into a 512 entry L2 table and each entry points to a 4kB block.

![1](../../../../../../../.\image\1.png)

**Memory attributes**

In arm64 address translation there are three different memory descriptors: table descriptor, block descriptor and page descriptor. 

- table descriptor: contains next_table_address with corresponding attribution
	- bits[1:0] == 0b11
- block: contains pfn with corresponding attribution(in this lab, we use block descriptor to organize the kernel page table) 
	- bits[1:0] == 0b01
- page descriptor: contains pfn with corresponding attribution(in this lab, we use page descriptor to organize the user page table) 
	- bits[1:0] == 0b11

The following figure shows how memory attributes are specified in a  block / page entry. The block / page entry in the translation table defines the attributes for each memory region.

![2](../../../../../../../.\image\2.png)

- **Unprivileged eXecute Never** (UXN) and **Privileged eXecute Never** (PXN) are execution permissions. 

	- UXN == 1 unprivileged program can not execute
	- PNX == 1 privileged program can not execute

- **AF** is the access flag. 

- **SH** is the shareable attribute. 

	| SH0 bits[13:12] | Cacheable Property                                         |
	| --------------- | ---------------------------------------------------------- |
	| 00              | Normal memory, Inner Non-cacheable                         |
	| 01              | Normal memory, Inner Write-Back Write-Allocate Cacheable   |
	| 10              | Normal memory, Inner Write-Through Cacheable               |
	| 11              | Normal memory, Inner Write-Back no WriteAllocate Cacheable |

- **AP** is the access permission. 

	| AP   | Unprivileged (EL0) | Privileged (EL1/2/3) |
	| ---- | ------------------ | -------------------- |
	| 00   | No access          | Read and write       |
	| 01   | Read and write     | Read and write       |
	| 10   | No access          | Read-only            |
	| 11   | Read-only          | Read-only            |

- **NS** is the security bit, but only at EL3 and Secure EL1.

- **Indx** is the index into the MAIR_ELn.

	- Indx == 0 strongly ordered device memory
	- Indx == 4 normal memory

There are several distinctions between page / block descriptor and table descriptor, the following figure shows the format of table descriptor:

![table_descriptor](../../../../../../../.\image\table_descriptor.png)

- **NSTable**: This bit indicates whether the table identifier in the descriptor is located in Secure PA space (NSTable == 0) or Non-Secure (NSTable == 1) memory for an access from Secure state

	If NSTable == 1, any value of NS bit on the page or block descriptor on any subsequent lookup is ignored and the block or page referred to is in Non-Secure memory. Also, the value of NSTable in any further looked up table is ignored and such tables’ entries refer to Non-Secure memory. Further, an entry fetched in the secure state is treated as nG == 1 (non-global).

- **APTable**: APTable bits set permissions on, at one level of translation, the referred entries at subsequent levels of lookup in the same stage of translation.

	For single exception level translation regimes (Secure EL3 and Non-Secure EL2), APTable bit 0 is RES0, making only the bit 1 valid. If set (APTable[1:0] = 0b1x), Write access is not pemitted at any exception level, irrespective of permissions on subsequent level lookup

- **UXNTable**: Stands for Unprivileged eXecute Never, UXN bit is treated as set in all subsequent levels lookup, irrespective of the actual value set in the descriptors

- **PXNTable**: Stands for Privileged eXecute Never, PXN bit is treated as set to 1 for all subsequent levels of lookup, irrespective of the actual value set in the descriptors

From code executing on the CPU, once we're in protected mode (which we entered first thing in `bootloader/arch64/init/init_c.c`), there's no way to directly use physical address. *All* memory references are interpreted as virtual addresses and translated by the MMU, which means all pointers in C are virtual addresses.

The Chcore kernel often needs to manipulate addresses as opaque values or as integers, without dereferencing them, for example in the physical memory allocator. Sometimes these are virtual addresses, and sometimes they are physical addresses. To help document the code, the Chcore source distinguishes the two cases: the type `vaddr_t` represents opaque virtual addresses, and `paddr_t` represents physical addresses. Both these types are really just synonyms for 64-bit integers (`u64`), so the compiler won't stop you from assigning one type to another! Since they are long types (not pointers), the compiler *will* complain if you try to dereference them.

The Chcore kernel can dereference a `vaddr_t` by first casting it to a pointer type. In contrast, the kernel can't sensibly dereference a physical address, since the MMU translates all memory references. If you cast a `paddr_t` to a pointer and dereference it, you may be able to load and store to the resulting address (the hardware will interpret it as a virtual address), but you probably won't get the memory location you intended.

To summarize:

| C type    | Address type |
| :-------- | :----------- |
| `T*`      | Virtual      |
| `vaddr_t` | Virtual      |
| `paddr_t` | Physical     |

> **Question**
>
> Assuming that the following CHcore kernel code is correct, which type should variable `x` have, `vaddr_t` or `paddr_t`?
>
> ```
>     mystery_t x;
>     char* value = return_a_pointer();
>     *value = 10;
>     x = (mystery_t) value;
> ```

However, the Chcore kernel sometimes needs to read or modify memory for which it only knows the physical address. For example, adding a mapping to a page table may require allocating physical memory to store a page directory and then initializing that memory. However, the kernel, like any other software, cannot bypass virtual memory translation and thus cannot directly load and store to physical addresses. One reason Chcore remaps of all of physical memory starting from physical address 0 at virtual address 0xFFFFFF0000000000 is to help the kernel read and write memory for which it knows just the physical address. In order to translate a physical address into a virtual address that the kernel can actually read and write, the kernel must add 0xFFFFFF0000000000 to the physical address to find its corresponding virtual address in the remapped region. You should use `phys_to_virt(pa)` to do that addition.

The Chcore kernel also sometimes needs to be able to find a physical address given the virtual address of the memory in which a kernel data structure is stored. When  traversal the page table, Chcore kernel will transform virtual address of page structure to physical address and fill it into the page table. Thus, to turn a virtual address in this region into a physical address, the kernel can simply subtract 0xFFFFFF0000000000. You should use `virt_to_phys(va)` to do that subtraction.

> **Question**
>
> 1. How much space overhead is there for managing memory, if we actually had the 4G physical memory? How is this overhead broken down?
> 2. Summarize the difference between X86 and ARMaddress translation mechanisms, what is the advantage of arm-smmu architecture design?

### Page Table Management

Now you'll write a set of routines to manage page tables: to insert and remove virtual-to-physical mappings(tranditonal 4-level page table), and to create page table pages when needed.

> **Exercise 2.** In the file `kernel/arch/aarch64/mm/page_table.c`, you must implement code for the following functions.
>
> ```
> map_range_in_pgtbl()
> unmap_range_in_pgtbl()
> query_in_pgtbl() 
> ```

You can invoke the assisted functions: `set_pte_flags()`,`get_next_ptp()`, `flush_tlb()` we have already implemented. Read the annotations carefully before using these functions

`test_map_unmap_page()` test your page table management routines. You should make the test files under the `/test/mm/page_table` using the following scripts:

```
make docker
cd ./test/mm/page_table
cmake ./
make
./test_aarch64_page_table
```

If you pass the all tests, you can see the output as follow::

```
1 tests, 82529 assertions, 0 failures
```

Run test_aarch64_page_table and see whether reports 0 failures. Fix your code so that it passes. You may find it helpful to add your own `mu_check()`s to verify that your assumptions are correct.

## Part 3: Kernel Address Space

Chcore divides the processor's 64-bit linear address space into two parts. User environments (processes), which we will begin loading and running in lab 3, will have control over the layout and contents of the lower part, while the kernel always maintains complete control over the upper part. The dividing line is defined somewhat arbitrarily by the symbol `KBASE` in `kernel/include/arch/aarch64/arch/mmu.h`.

### Permissions and Fault Isolation

Since kernel and user memory are both present in each environment's address space, we will have to use permission bits in our arm64 page tables to allow user code access only to the user part of the address space. Otherwise bugs in user code might overwrite kernel data, causing a crash or more subtle malfunction; user code might also be able to steal other environments' private data. 

The user environment will have no permission to any of the memory above `KBASE`, while the kernel will be able to read and write this memory.  the address space below `KBASE` is for the user environment to use; the user environment will set permissions for accessing this memory.

### Map the Kernel Address Space

In the boot time, Chcore already map the virtual address [KBASE : KBASE + 128M] to physical address [0 : 128M], in this lab we will map  additional 128M virtual address[KBASE + 128M: KBASE + 256M] to physical address[128M : 256M]. Upper to KBASE + 256M, the virtual memory is reserved for device.

| virtual address                                              | physical address |
| ------------------------------------------------------------ | ---------------- |
|                                                              |                  |
| ...                                                          |                  |
| KBASE+256M : KBASE+4G(ttbr1) (strongly ordered device memory) | 256M : 4G        |
| KBASE+128M - KBASE+1256M (kernel space hole fix in this lab) | 128M : 256M      |
| KBASE - KBASE+128M (ttbr1) (normal memory)                   | 0 : 128M         |
|                                                              |                  |
| ...                                                          |                  |
| 2G : 4G (ttbr0) (strongly ordered device memory)             | 2 : 4G           |
| 0 : 2G (ttbr0) (normal memory)                               | 0 : 2G           |

 

Chcore use block to manage the kernel memory, review what you have learned in last part. Each block contains 1G / 2M physical memory, so when translating the virtual address to physical address in kernel space, mmu only needs to traverse 2-level  / 3-level page table(2-level page table for 1G block and 3-level page table for 2M block). The first level page table(pgd) address is store in the ttbr1 register, the second level page table(pud) address is stored in the pgd entry, similarly, pmd address is stored in the pud entry.

> **Question** 
>
> 1. Why CHcore organize the kernel page table in  block entry? Which range of virtual address space must map in the boot time and which can delay until the kernel starts?
> 2. Why will user programs not be able to read or write the kernel's memory? What specific mechanisms protect the kernel memory?

EL0 has no permission to read / write / execute the memory in kernel space, so the attribution in block entry(pud / pmd) is:

- UXN = 1
- AF = 1
- SH = 3
- Indx = 4
- bits[1:0] = 1 (describe for block entry)

You can read the ttbr1 register to get the current first level page table(pgd) address.

```
unsigned long get_ttbr1()
{
    unsigned long pgd;
    __asm__ ("mrs %0,ttbr1_el1" : "=r"(pgd));
    return pgd;
}
```

> **Exercise 3.** Fill in the missing code in `map_kernel_space()` 
>
> Your code should now pass the `kernel_space_check()`

**Challenge 1.** Map the kernel space in the page granularity. In default, only user process can map virtual address in the page  granularity, so you need to  modify the function `set_pte_flags()`.

**Challenge 2** Support hugepage / block  for Chcore, you should modify the map_range_in_pgtbl() in page_table.c and set the page attribution bit carefully. 

If you complete any of the challenges, you need to explain your solution in the document, what's more we  recommend that you write your own test scripts to check whether you have implemented it correctly, you can get the extra **bonus** after finishing all challenges.

**This completes the lab.** Type `make grade` in the `lab` directory for test. If you pass all tests, you can see your grade report as follow:

```
running chcore: (0.2s) 
  buddy: OK 
  page table: OK 
  kernel space check: OK 
Score: 100/100
```

Hand in your lab with git command:

```
git add .
git commit -m "finish lab2"
git push -u origin
```

