# Homework 8

## Question 1

### Question

What are the three fundamental requirements addressed by computer security?

### Answer

**Confidentiality**, **Integrity**, and **Availability**.

## Question 2

### Question

What is the relation between access control list and capabilites?

### Answer

Access control list stands on resources' view, determining who can access it and who can't.

Capabilities stands on users' view, determining what resources it could access and what it couldn't.

## Question 3

### Question

What's the threat model of enclave? Why enclave's threat model is reasonable? When designing enclave, why do we must be careful of side-channel attacks?

### Answer

The threat model of **enclave** is all hardware except CPU (with its cache) are all evil, controlled, or attacked.

Since in this threat model, all devices except CPU are suspicious, so we must be careful not to trust any external devices. If we accidentally expose any vulnerable points, it will very likely to be noticed and attacked by any side devices.

## Question 4

### Question

When doing pressure testing on I/O, do we need to saturate the the other resources (memory, CPU)?

### Answer

I don't think we should saturate that. If we saturate memory and CPUs, then the bottleneck might not be the target factor, and the result won't be distinguishing enough.

## Question 5

### Question

In fuzzing test, syzkaller generates different inputs to the operating system by constructing system calls in user mode. Is there any other way to generate input for the operating system?

### Answer

Except system calls, we can also deliberately raises *page fault* exceptions, *bad instruction* exceptions, *divided by zero* exceptions, etc.

In one sentence, we can deliberately raise some synchronized exceptions to fuzzing test our kernel.