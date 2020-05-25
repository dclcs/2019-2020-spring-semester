# SE-315

## Lab 4 Report

### Part A: Multiprocessor Support

#### Question 1

Here's the full result of command `git diff lab3 ./boot/start.S`:

```diff
diff --git a/boot/start.S b/boot/start.S
index 1ec7118..61d4cc8 100755
--- a/boot/start.S
+++ b/boot/start.S
@@ -1,13 +1,17 @@
 #include <common/asm.h>
 
 #define __ASM__
-#include "../lib/registers.h"
+#include <lib/registers.h>
 #undef __ASM__
 
 .extern init_c
 .extern arm64_elX_to_el1
 
+.extern vecotrs
 .extern boot_cpu_stack
+.extern secondary_boot_flag
+.extern secondary_init_c
+.extern MPIDR
 
 BEGIN_FUNC(_start)
   /* get the processor id, and store it in register x8 */
@@ -16,15 +20,49 @@ BEGIN_FUNC(_start)
        bic     x8, x8, x9
        cbz     x8, primary
 
-  /* hang all secondary processors before we intorduce multi-processors */
-secondary_hang:
-       bl secondary_hang
+       /* Turn to el1 from other exception levels. */
+       bl      arm64_elX_to_el1
+
+       /* Find the logical cpu id */
+       ldr     x1, =MPIDR
+       mov     x0, #0
+find_id:
+       ldr     x2, [x1, x0, lsl #3]
+       add     x0, x0, #1
+       cmp     x2, x8
+       bne     find_id
+
+       /* x8 stores the logical cpu id */
+       sub     x8, x0, #1
+
+       /* Prepare stack pointer and jump to C. */
+       mov     x1, #0x1000
+       mul     x1, x8, x1
+       adr     x0, boot_cpu_stack
+       add     x0, x0, x1
+       add     x0, x0, #0x1000
+       mov     sp, x0
+
+hang:
+       /* CPU ID in x8 */
+       mov     x1, #8
+       mul     x2, x8, x1
+       ldr     x1, =secondary_boot_flag
+       add     x1, x1, x2
+       ldr     x3, [x1]
+       cbz     x3, hang
+
+secondary_continue_boot:
+       /* Set CPU id */
+       mov     x0, x8
+       bl      secondary_init_c
 
 proc_hang:
-       bl proc_hang
-       
+       bl      proc_hang
+
+
 primary:
-  /* Turn to el1 from other exception levels. */
+       /* Turn to el1 from other exception levels. */
        bl      arm64_elX_to_el1
 
        /* Prepare stack pointer and jump to C. */
@@ -32,10 +70,6 @@ primary:
        add     x0, x0, #0x1000
        mov     sp, x0
-       /* Set up boot exceptions vector */
-       #adr    x0, boot_vector
-       #msr    vbar_el1, x0
-
        bl      init_c
 
        /* Should never be here */
```

Here's some major changes for `start.S`:

##### More formal `#include` format

Now, the `register.h` header file is wrapped by `<%s>` rather than `"%s"`. That indicates that this header file is part of system library.

Actually, since in `CMakeLists.txt`, line 48, it says:

```cmake
set(KERNEL_LIB_PATH         "${CMAKE_CURRENT_SOURCE_DIR}/lib")
```

And any files located in `lib` can be referenced in the compiler's system library. Using `<%s>` is more specific than relative-path based `"%s"`.

##### Hang secondary processors no more

As declared in previous labs, any secondary processors' control flow will be redirected to the label `secondary_hang`, and suffer eternal karma. However now, they will be all set to `EL1` exception level, and intiailize independent kernel stack. However, only the main processor will do the most important `init_c` stuff.

#### Exercise 2

##### Trigger it

According to the hang code in `start.S`, the control flow will stuck in this situation:

* `x2` contains `#8 * CPU_ID`
* `x1` contains `&secondary_boot_flag`
* calculates `*(secondary_boot_flag + 8 * CPU_ID)`
* if the result is zero, keep looping
* otherwise, go to `secondary_init_c`

Obivously, we just need to set a flag in `(void *)secondary_boot_flag + 8 * i`, writing whatever non-zero bytes here to activate those secondary processor.

##### Do it sequentially

However, by executing this we notice that processors are activated in unexpected sequence.

In order to activate it sequentially, we can use `cpu_status` buffer.

It can contain 4 `char` objects. More than that, it's declared as `volatile`. That means it will not be stored in any processor-dependent registers, but the shared main memory. So we can exchange information across processors with its help.

Once a processor hits `secondary_init_c`, we update its corresponding `cpu_status` array with `cpu_run` flag. `enable_smp_cores` will not activate more processors until the `cpu_status` gets updated.

#### Question 3

##### Check memory race

According to `start.S`, each processor's kernel stack is actually independent: they're allocated in neighboring memory blocks, but they're actually independent (not considering over-boundary r/w).

So there should be no memory race here.

##### Check control flow race

During each processor's activation period, it mainly calls two functions: `el1_mmu_activate` and `secondary_cpu_boot`.

For `el1_mmu_activate`, the comment in `tools.S`: line 270 has clarified it well.

```assembly
/* The assembly only activates the mmu of the CPU calls it */
```

In its implementation, it didn't use any cross-processor variables/methods. So this function won't cause any control flow race.

However, for `secondary_cpu_boot`, things are different. it loads the content of `kernel_stack`, and use it to calculate its kernel stack address.

That will not cause control flow race, though. They're just loading the constant value `kernel_stack`, which won't be modified at all.

So, no control flow race will occur if we simultaneously activates all processors.

#### Exercise 4

First, we're expected to implement a simple ticket lock system.

Since `fetch_and_add` atomic function has been implemented as a macro, there's no much code for us to write.

#### Exercise 5

Just fill in the blanks.

Notice that by default we should obtain the *BIG KERNEL LOCK* once we initialize it, otherwise it will hit a `BUG_ON` assertion error.

#### Question 6

Main reason is those handler-used registers will never be used after the exception handler completes.

However, those registers before the exception triggering events should be stored well, since after the exception handler completes, those registers will be used, still.

### Part B: Scheduling

#### Exercise 7

