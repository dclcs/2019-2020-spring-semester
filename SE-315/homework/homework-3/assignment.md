# **Homework 3: IPC**

1. Please give a suitable real-world program example for each of the following IPC implementations:
   - Two processes communicate with each other by polling some data in shared memory
   - One sender process and one receiver process communicate with each other using non-blocking message passing
   - One sender process and one receiver process communicate with each other using blocking message passing
   - One sender process and multiple receiver processes communicate using a shared mailbox
2. In the Xv6 pipe implementation, what is the usage of the field `lock` in `struct pipe`? Why must we put the `release/acquire` operation inside the `sleep` function? Why is there no lock operation in the `wake` function?
3. For shared memory based IPC, there is a typical security problem named "Time-of-check to time-of-use". Please give a brief description of what this problem is with the help of internet and give a possible solution for this problem. **HINT**: the memory mapping mechanism may help.
4. For the LRPC implementation, please answer the following questions:
   - Why is the stack divided into an argument stack and an execution stack?
   - Where does the overhead come from during the control flow transformation of LRPC?
   - Are there any security problems caused by the shared argument stack? (Multi-threading are not considered)
5. Most IPC implementation requires a naming mechanism to find the specific process for communication (e.g., the `nread/nwrite` pointer in the Xv6 example). Will the design of the naming mechanism dominate the IPC performance? Why?
6. The performance of IPC heavily depends on the hardware architecture (e.g., how the hardware change privilege level and the hardware details of page table switching). For the IPC implementation of ChCore under AArch64 architecture, try to analyze what hardware may influence to the IPC performance. If you are a hardware designer hired by the ChCore team, what modification on the hardware can you do to speed up ChCore's IPC while not compromise security?