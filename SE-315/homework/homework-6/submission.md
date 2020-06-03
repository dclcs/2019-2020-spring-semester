# Homework 6

## Question 1

### Question

What are the main abstractions in Linux device driver model? Please navigate the `sysfs` ﬁlesystem and ﬁnd out their relationships.

### Answer

#### Abstractions

The main abstraction of LDDM is a general description of system architectures.

A general LDDM object should handle these tasks:

* Power Managing & System Poweroff
* Communicate with user space
* Handle Hot Plug
* Device Category
* Manage Object Life Cycle

#### `sysfs`

As a virtual memory file system, it is always mounted to `/sys`.

## Question 2

### Question

ARM Generic Interrupt Controller (GIC) indicates an interrupt's completion using two phases: (i) priority dropping and (ii) interrupt deactivation? How does it relate to the interrupt handling mechanism in Linux, i.e., top and bottom halves?

### Answer

When GIC detects an `INACTIVATE` interrupt, it will mark it `PENDING`. Then, the distributor model will decide its CPU affinity and make it `ACTIVE`, and write some specific registers to invoke the interruption.

Notice that during this period, multiple interruptions might come at the same time, carrying different privilege levels. GIC will block those interruptions that doesn't exist in masking list, and choose the highest prioritized one to send to CPU.

However, even a GIC interruption with high priority level can't interrupt a lower one currently handling in CPU, since CPU's interruption handling switch is off until it's handled.

So GIC + Linux's interruption strategy isn't a fully prioritized one.

## Question 3

### Question

In the scenario of data transmission, what will happen if the receiver cannot catch up with the sending speed of the transmitter? How to handle this? (Hint: think about the choice and consequence between polling and interrupts)

### Answer

If the packets are sent too quickly to be fully received by the receiver...

* If it's a polling strategy based transmission, some packets will be lost if they're sent too frequenty
* If it's a interrupt strategy based transmission, some packet sending program might hang until the last packet got handled

How do we handle this?

* we should use more than one thread to receive data packet
* at the same time, we might extend buffer size to avoid buffer explode scenario

## Question 4

### Question

Describe the dataﬂow of how an application receives a network packet from the network card (NIC). In particular, how many context switches are needed in Linux? What about ChCore?

### Answer

Basically, it's a process copying data from network card hardware buffer to memory.

First, after network card receives a packet, it will trigger a hardware interruption to announce copying a new packet to handle. After that the network card driver will be called and that will do some validation work. After that, it triggers a software interruption to tell the system to copy data from the buffer.

For ChCore's implementation, it probably uses a asynchronized packet transmission design. After a packet is brought out from NIC buffer, the hardware interruption switch will be turned on again for more packets coming. And those temporary packets will be put into a ring buffer space, waiting to be handled by another soft interruption coming from ChCore's microkernel. Thus, hard and soft interruption will not be strictly bound to each other, and they could work simultaneously to some extent (if the ring buffer is big enough).

## Question 5

### Question

How does Linux skb achieve zero copy in kernel space? When does `skb_copy` have to occur? Please consider shallow copy (`skb_clone`) and deep copy (`skb_copy`).

### Answer

* Zero copy between Drivers & Kernel can be achieved by pre-allocating `sk_buff` structures and reuse them during the data transmission process
* Zero copy between Processes & Kernel can be achieved by neatly remapping specific kernel memory phases to user space

## Question 6

### Question

What techniques does Intel DPDK leverage to improve network performance? If you are a network designer hired by the ChCore team, what would you like to choose to optimize ChCore's microserver-based network server? You may also learn from "Snap: a microkernel approach to host networking", Marty et al., SOSP’19.

### Answer

Intel thinks that the major performance lagging comes from the kernel-space switch. So it decides to bypass OS kernel and implement data trasmission purely in user mode.

ChCore's network implementation still relies on IPC messaging between user-mode NIC driver and its microkernel. If we're going to introduce similar techniques like DPDK, we should implement a quick, light and safe data transmission technique among user-model applications, without any kernel switching at all.

## Linux Modules

```
 |------------> M M <------------|
 |               ^               |
 |               |               |
 v               v               |
VFS ---------> SCHED <--------- IPC
 |
 |
 |----------->  NET
```

* `SCHED` evenly manages CPU resources
* `MM` ensures each process' memory managing safety
* `VFS` is the illuson of all extern devices
	* including disks, FLASH drives, I/O devices and display devices
* `NET` manages all network stuff
* `IPC` manages inter-process communication stuff

