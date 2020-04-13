# SE-315

## Lab 2 Report

### Part 1: PPM

#### Question 1

##### Q

Make a brief explanation: In which file or code segment specifies the ChCore physical memory layout. You can answer this question in two aspects: the compilation phase and runtime phase?

##### A

First, the reversed memory size was defined as a macro in file `./boot/image.h`, aka `TEXT_OFFSET`.
Then it was referenced in `linker-aarch64.lds.in`, determining a global variable `img_start`.

Similarly, `./boot/image.h` defines macro `KERNEL_VADDR`, and `. - KERNEL_VADDR` is the `img_end`, which is an alias of `metadata_start`.

They are all determined after the compilation.

`metadata_end`, aka `page_start`, is calculated by adding the size of all memory pages' meta data (Page Count $\times$ `sizeof(struct page)`) and the `img_end`.

Similarly the `page_end` is calculated by adding the size of memory pages (Page Count $\times$ `PAGE_SIZE`).

> By the way, `PAGE_SIZE` is defined in `./kernel/common/mm.h`.

Those memory phases are determined in runtime. Related codes lies in `./kernel/mm/mm.c`.

#### Exercise 1

In this phase, we merely needs to modify three functions. They are `buddy_get_pages()`, `__alloc_page()`, and `buddy_free_pages()`.

In my implementation (mainly based on Linux), `buddy_get_pages()` merely validates input `order`, calls internal `__alloc_page` function to obtain `page *`, and modifies its `order` entry.

`__alloc_page()`, seemingly like an internal function, does the most dirty work.  It checks buddy system's free list from `target_order` till `BUDDY_MAX_ORDER`. Once it obtains one free block, check if its order is our target order. If so, great. Just `clear_page_order_buddy` and return that page. If not so, we should recursively call `split` the `higher_order` into `target_order`, and then return that page.

`buddy_free_pages()`, mainly do the page release job. After a page is returned, we should add that page back into the free list. Moreover, we should check if this page has a free buddy. If so, coalesce them recursively.

We can use predefined `get_buddy_page()` and `check_buddy()` to find and identify its buddy page.

It should noticed that `clear_page_order_buddy` removes the page's order field. So we should manually set this field before returning it in ``buddy_get_pages()`.

### Part 2: VM

#### Question 2

##### Q

Assuming that the following ChCore kernel code is correct, which type should variable `x` have, `vaddr_t` or `paddr_t`?

```c
mystery_t x;

char* value = return_a_pointer();

*value = 10;

x = (mystery_t) value;
```

##### A

It should be `vaddr_t`. Since it can be dereferenced as a pointer.

#### Question 3

##### Q

How much space overhead is there for managing memory, if we actually had the 4G physical memory? How is this overhead broken down?

##### A

Technically, if we have to save all remapping relations for each physical location, or 4294967296 entries, each one contains 128 bits, or 16 Bytes, we will need 68719476736 extra bytes to save all these relations, which is unacceptable.

However, if we follow the rule that in kernel's memory space, we simply needs to make a addition or subtraction with the help of address offset `0xFFFFFF0000000000` in order to convert from PMA to VMA.

#### Question 4

##### Q

Summarize the difference between x86 and ARM address translation mechanisms, what is the advantage of arm-smmu architecture design?

##### A

The major advantage that ARM holds over x86 is the completed privilege control strategy.

For x86 there are only one single bit for accessing privilege control: the `R/W` bit. Worse than that, this bit won't constrain all accessing coming from Ring 0, 1, and 2, which is a potential vulnerability. And there's no such thing could indicate if this memory page could be executed as codes, or shouldn't be executed as data. X bit is introduced in x86-64 to fix that problem.

However, ARMv8 has a complete privilege control system, containing a complete R/W/X privilege managing strategy. And it could separatedly configure accessing privileges in different privilege levels.

#### Exercise 2

Basically, `query_in_pgtbl()`, `map_range_in_pgtbl()`, and `unmap_range_in_pgtbl()` do the same thing. They call `get_next_ptp()` 4 times to get its results.

Notice that in each page table entry (`pte_t`), `pte_t.pte` contains not only next page table entry, but also some state bits. In order to split memory address out, we should use `(u64)pte_t.table.next_table_addr << PAGE_SHIFT` to obtain that.

Notice that `next_table_addr` is merely `u32`. Before shifting it we should expand that to `u64` in case of overflow.

### Part 3: KAS

#### Question 5

##### Q

Why ChCore organize the kernel page table in block entry? Which range of virtual address space must map in the boot time and which can delay until the kernel starts?

##### A

Since kernel's memory accessing is quite localized, it is cheaper to reduce memory translating process as much as possible, and they're more likely to be cached in its personal TLB.

Those virtual address containing kernel codes and kernel stacks should be mapped immediately when OS boots up. Those containing the page table stuff could wait till the system is actually running up.

#### Question 6

##### Q

Why will user programs not be able to read or write the kernel's memory? What specific mechanisms protect the kernel memory?

##### A

Since all user programs' kernel stack maps to the same kernel physical address, one program manipulating kernel will affect all other programs, or break down the whole system if worse.

All high-privileged operations should only be performed by OS exposed system call interfaces.

In order to keep such permission check, some OS has an integrity check. If a user-program tries to access an obviously incorrect memory address (like, those starts with `0xfffff....`), it will simply raise a segmentation fault and terminate this program.


#### Exercise 3

