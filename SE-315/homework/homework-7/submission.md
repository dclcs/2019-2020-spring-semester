# Assignment 7

## Question 1

### Question

Assume there are 20 virtual machines running on a physical machine and 20 processes in each virtual machine. If we are using the shadow page table for memory virtualization, how many shadow page tables are needed? If we are using the stage-2 page table for memory virtualization, how many stage-2 page tables are needed?

### Answer

* 400 shadow page tables are needed
* 20 stage-2 page tables are needed

## Question 2

### Question

In many cases, the stage-2 page table performs better than the shadow page table. However, in some cases, the shadow page table can outperform the stage-2 page table. Please give an example of such cases and explain why.

### Answer

If there's a lot of single-thread virtual machines running on a single machine, which means the amount of page tables won'd differ a lot whether we choose shadow page tables or stage-2 page tables. However, for stage-2 page table's situation, we have to create full 4-level page table. But for shadow page table strategy, we just need to construct the deepest level of the page table. So the cost can be reduced, and the performance can be optimized.

## Question 3

### Question

IOMMU is a wildly used technology in IO virtualization. What is the design purpose of IOMMU? In AArch64, IOMMU is implemented by SMMU. What address (HPA, HVA, GPA, GVA) are stored in SMMU? What kind(s) of translation (transforming what kind of address into what kind of address) can SMMU do?

### Answer

The purpose of IOMMU is providing an extra layer between I/O devices and physical memory, avoiding it to access a specific physical address.

With the help of IOMMU, developing drivers can be simplified quite a lot. I/O devices can simply access a kernel-space address, and this would be sent along the bus, going through the IOMMU, which converts it to a correct physical address.

SMMU can convert GVA => GPA, or GPA => HPA, or GVA => HPA.

## Question 4

### Question

Please sort container, AWS Firecracker, gVisor, and traditional virtual machine according to their security level and give your reason.

### Answer

Container: #4, provides resources isolation only

AWS Filecracker: #3, different microVMs uses the same KVM 

gVisor: #1, fully kernel running in user mode

Traditional Virtual Machine: #2, virtual machine still have many mechanics directly involve with host

## Question 5

### Question 

Assume we are going to implement container support (both CGroup and namespaces) in ChCore in Lab6. (*Don't worry, there won't be such a lab in this semester*). What modification should be done? Are there any performance or programmability advantages to implement such supports in ChCore compared to Linux due to ChCore's architecture?

### Answer

A lot of modifications are needed:

* the file system must be changed
  * currently, our simple memFS didn't provide any authorization and authentication support at all.
* more `syscall` to be added
  * all resource requests should be done by `syscall`
  * and provide its corresponding `PID` for identification
* the memory allocating portion should be modified, in the same way
  * since memory as a kind of resource must be restricted
* most importantly, the `PID` and `PGroup` feature should be fixed a lot
  * or there would be no way to identify a process' resource quota or security level, etc.

If there's any kind of advantages of ChCore over Linux...

* Currently ChCore has significantly less codes and complications than Linux, so modification is simplier and easier.
* ChCore is a micro kernel indeed, so actually file systems are implemented as services, so modificating them will be easier.

