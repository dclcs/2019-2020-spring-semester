# **Homework #6: Device and Network**

1. What are the main abstractions in Linux device driver model? Please navigate the sysfs ﬁlesystem and ﬁnd out their relationships.

2. ARM Generic Interrupt Controller (GIC) indicates an interrupt's completion using two phases: (i) priority dropping and (ii) interrupt deactivation? How does it relate to the interrupt handling mechanism in Linux, i.e., top and bottom halves?
3. In the scenario of data transmission, what will happen if the receiver cannot catch up with the sending speed of the transmitter? How to handle this? (Hint: think about the choice and consequence between polling and interrupts)

4. Describe the dataﬂow of how an application receives a network packet from the network card (NIC). In particular, how many context switches are needed in Linux? What about ChCore?

5. How does Linux skb achieve zero copy in kernel space? When does skb copy have to occur? Please consider shallow copy (`skb_clone`) and deep copy (`skb_copy`).

6. What techniques does Intel DPDK leverage to improve network performance? If you are a network designer hired by the ChCore team, what would you like to choose to optimize ChCore's microserver-based network server? You may also learn from "Snap: a microkernel approach to host networking", Marty et al., SOSP’19.