# Assignment 1

Assuming a 64-bit machine with 64-bit OS with a 4-level page table. Level-4 (`L4`) is PGD, `L3` is PUD, `L2` is PMD, `L1` is PTE.

## Question 1

### Q

On a 32-bit machine, the huge page size is 4M. On a 64-bit machine, the size of a huge page is 2M. Why not the same size?

### A

Because for a 64-bit machine, we use `PMD` page table to store Huge Physical Page's memory address. So, the last page table's size (which equals to $512 \times 4 KiB = 2 MiB$) is the size of each huge page.

## Question 2

### Q

Why OS/MMU use multi-level page tables to map the virtual address to physical address? What is the maximum spatial overhead for the 4-level page table?

### A

Because **a)** we didn't save both physical page ids and virtual page ids, but only sequentially put physical page ids altogether. That means we have to save all physical page ids if we merely use single-level page table, most of which are useless empty mapping, though.

And **b)**, multi-level page tables can exponentially reduce total page table size. Some empty memory chunks will be ignored at a high level.

Consider a normal 64-bit machine, its addressing space is no less than $2^{48}$ Bytes. That means as a typical 4K page size will require at least $2^{36}$ page entries, which is a great waste.

However, if there's actually only 1 necessary page entry, we just need 4 page tables to complete that in other way, which equals to $4 \times 2 ^ {20}$ page entries.

So, the `4-Level:Single` ratio equals to $\dfrac {4 \times 2 ^ {20}} {2^{36}}$, also $2^{-14}$. Approximately $0.006103515625 \%$.

So the maximum spaical overhead for 4-level page table is approximately $99.9938965 \%$.

## Question 3

### Q

In ARM-smmu architecture, how can mmu distinguish whether it is a page entry or invalid entry?

### A

Each page has its own protection bits, mainly configures each user's r/w/x privileges. When MMU perform the address translation procedure, it will also check if current user has proper privilege to r/w/x this page. If not, it will reject further translation process and raise an exception. Physical address will be provided only if current application has proper privileges.

## Question 4

### Q

Please give the advantages and the disadvantages of using block entry / huge page, and give one scenario for each case.

### A

Advantages: 

* Reducing fragmentations. 
    
> Default memory page (4K) is way too small, compared with current large memory size. So if we use several *huge page* to reduce hundreds of normal page, we can easily reduce those abstract objects greatly.

* Speeding up address translations.

> In order to access a large chunk of physical memories, it's always better making it a consecutive large chunk than splattered memory smashes. It could reduce duplicate address translation processes, and ensure they will be literally consecutively spread in physical memory.

Disadvantages:

* Memory waste.

For those application requests little amount of memories, allocating a whole huge page is actually a waste.

## Question 5

### Q

Memory attribution bit AP and UXN in page entry can already isolate the kernel space and user space, so why ARM-smmu architecture still needs two ttbr registers (Translation Table Base Register), please give a scenario that two ttbr registers can protect the os but attribution-based isolation can not.

### A

For example, if theres a bug in OS kernel that affects normal control flow, it might accidentally access user-mode page table, ignoring AP and UXN bits as high privilege level. However, there's no way confusing user and kernel pages if we use two physically-isolated `TTBR` registers.

## Question 6

### Q

TLB (Translation Lookaside Buffer) can cache the translation from a virtual address to a physical address. However, TLB will be flushed after a context switch between processes. Why? Is it necessary to flush TLB when switching between a user application and the OS kernel? Why?

### A

Because different processes might share identical virtual memory addresses. If we didn't flush TLB before switching context, you may accidentally access other process' memory space.

However, OS kernel's address space exists in each process' context and will never overlap with user's address space. So there's no need to flush TLB during that process.

## Question 7

### Q

Before ARMv8 architecture, there is no DBM (Dirty Bit Modifier) bit in memory attribution. That’s means hardware does not support dirty page. So how can os simulate this procedure and record the dirty page? Please give a possible solution.

### A

We can use existed `Read Only` to implement DBM feature. By setting this bit, any reading instructions' translation related to this page will be fine, but trying to write to this page will invoke an exception. In such exception handler, we can record the read/write history in kernel, and unset its `Read Only` bit to allow further clean writing.