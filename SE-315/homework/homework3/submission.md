# Assignment 3

## Question 1

### Q

Please give a suitable real-world program example for each of the following IPC implementations:

- Two processes communicate with each other by polling some data in shared memory
- One sender process and one receiver process communicate with each other using non-blocking message passing
- One sender process and one receiver process communicate with each other using blocking message passing
- One sender process and multiple receiver processes communicate using a shared mailbox

### A

Program Examples:

* Imagine a multi-processor supported logging application.
  * In order to accelerate the computing process, it's reasonable to spread the workload evenly to CPU processors.
  * However, all logging infos should be printed all in one. So all processor cores should pop out logging data in a chunk of shared memory.
* Imagine a pipeline based program series with buffer.
  * In a complete pipeline program series, different phases of program works individually.
  * So, it's necessary to use message passing methods to inform the next phase in the pipeline chain that it should do its work.
  * Moreover, it's not useful and efficient to use blocking message passing method, since that would cause great lagging in the whole pipeline. So we should use non-blocking message passing implementation.
* Imagine a pipeline based program series without buffer.
  * It's basically the same as the second one, however, in this case we don't have a buffer to temporarily contain mid-fabs.
  * So, the former phase must explicitly wait for the latter phase, ensuring it has obtained data, and then processing the next phase.
  * Such pipeline series' performance must be poor.
* Considering actual OS interfaces, there are lots of registered "Notifications" or "Services".
  * Some programs might register a notification provided by OS, requiring it being notified when specific events occurred.
  * For example, some program might want to be informed when system time is changed. They would use system API to register a notification and get a specific mailbox.
  * So when OS triggered an event, all user applications holding this mailbox should be notified.

## Question 2

### Q

In the Xv6 pipe implementation, what is the usage of the field `lock` in `struct pipe`? Why must we put the `release/acquire` operation inside the `sleep` function? Why is there no lock operation in the `wake` function?

### A

Field `lock` is a simple spinlock. It forces reading and writing operations atomic, avoids their racing faults.

Because before we `sleep` any process, it's necessary to release all locks it is currently containing. Otherwise, deadlock situation will be raised.

Since after `wake` was called, the executing control flow will not alter from current process. It just indicates that those processors could be scheduled in a later context switching. So there's no need to do any lock operations here.

## Question 3

### Q

For shared memory based IPC, there is a typical security problem named "Time-of-check to time-of-use". Please give a brief description of what this problem is with the help of internet and give a possible solution for this problem. 

**HINT**: the memory mapping mechanism may help.

### A

Corresponding [Wikipedia Page](https://ja.wikipedia.org/wiki/Time_of_check_to_time_of_use) Referred.

First, it's quite easy for us to write some codes like this:

```c++
if (access("file", W_OK) != 0) {
   exit(1);
}

fd = open("file", O_WRONLY);
write(fd, buffer, sizeof(buffer));
```

Checking the accessibility, and then write to it. It seems so natural, but it's problematic. The main reason is we can't ensure if the `file` keeps unmodified between the time we check and the time we access it.

For example, if we perform an attack like this:

```shell
# 権限チェックのあとで以下を実行する。
symlink("/etc/passwd", "file");
# これによって "file" はパスワードファイルを指すことになる。
```

And if this instruction is performed between code line 3 and code 5, the checking will be invalid when we actually use it.

```c++
if (access("file", W_OK) != 0) {
   exit(1);
}

fd = open("file", O_WRONLY);
// 実際には /etc/passwd を上書きしてしまう。
write(fd, buffer, sizeof(buffer));
```

That's because our `check` and `use` operation series aren't atomic, attackers might do evil things between our normal operation series.

The solution is to record the time between `check` and `use` (ToCtToU), making sure there's no unreasonable time elapsing.

In actual coding, we can use CPU instruction `rdtsc` to calculate time in high precision.

In POSIX, we can use `clock_gettime` and in Win32 there are `QueryPerformanceCounter` and `QueryPerformanceFrequency`.

Precision is so high that it can even indicate each CPU cycle.

## Question 4

### Q

For the LRPC implementation, please answer the following questions:

- Why is the stack divided into an argument stack and an execution stack?
- Where does the overhead come from during the control flow transformation of LRPC?
- Are there any security problems caused by the shared argument stack? (Multi-threading are not considered)

### A

* In LRPC implementation, the caller and callee should be entirely isolated. So, caller shouldn't know any information about the callee, including its execution stack. Their only sharing part is the argument stack. Separating them can make the isolation better.

* The main overhead comes from the kernel trap in/out.
* If a handler forgot to return in some case, the stack will keep unbalanced and the control flow will also alter incorrectly.

## Question 5

### Q

Most IPC implementation requires a naming mechanism to find the specific process for communication (e.g., the `nread/nwrite` pointer in the Xv6 example). Will the design of the naming mechanism dominate the IPC performance? Why?

### A

I don't think such mechanism would affect performance at all. Since they're just unique identifiers for each process, there won't be any difference from them if we use a `map` to convert among those idenfitying naming mechanism in $O(1)$ time.

## Question 6

### Q

The performance of IPC heavily depends on the hardware architecture (e.g., how the hardware change privilege level and the hardware details of page table switching). For the IPC implementation of ChCore under AArch64 architecture, try to analyze what hardware may influence to the IPC performance. If you are a hardware designer hired by the ChCore team, what modification on the hardware can you do to speed up ChCore's IPC while not compromise security?

### A

* Design a hardware method to forcefully override the default scheduling mechanism. Thus, we can accelerate the process switching.
* Provide an extra register to hold the capability bits of all IPC stuff. Thus we don't have to access the main memory to check the capability. Accessing register can be much faster.