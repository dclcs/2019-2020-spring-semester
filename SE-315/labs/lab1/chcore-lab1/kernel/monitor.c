/*
 * Copyright (c) 2020 Institute of Parallel And Distributed Systems (IPADS), Shanghai Jiao Tong University (SJTU)
 * OS-Lab-2020 (i.e., ChCore) is licensed under the Mulan PSL v1.
 * You can use this software according to the terms and conditions of the Mulan PSL v1.
 * You may obtain a copy of Mulan PSL v1 at:
 *   http://license.coscl.org.cn/MulanPSL
 *   THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND, EITHER EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT, MERCHANTABILITY OR FIT FOR A PARTICULAR
 *   PURPOSE.
 *   See the Mulan PSL v1 for more details.
 */

// Simple command-line kernel monitor useful for
// controlling the kernel and exploring the system interactively.

#include <common/printk.h>
#include <common/types.h>

static inline __attribute__((always_inline)) u64
read_fp()
{
    u64 fp;
    __asm __volatile("mov %0, x29"
                     : "=r"(fp));
    return fp;
}

__attribute__((optimize("O1"))) int
mon_backtrace()
{
    printk("Stack backtrace:\n");

    // get current frame pointer
    u64 *fp = (u64 *)read_fp();

    // ignore mon_backtrace's call
    fp = (u64 *)*fp;

    const u64 mask = 0xffffff0000000000;

    while ((u64)fp & mask)
    {
        printk("  LR %p  FP %p Args", *(fp + 1), fp);

        // after FP and LR, there should be spaces for local vars and saved regs
        u64 *arg_first = fp - 2;

        unsigned int arg_cnt = 0;
        while (arg_cnt < 5)
        {
            printk(" %p", *arg_first);
            --arg_first;
            ++arg_cnt;
        }

        printk("\n");

        fp = (u64 *)*fp;
    }

    return 0;
}
