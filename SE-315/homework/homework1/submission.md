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

