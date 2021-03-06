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

#include <exception/irq.h>
#include <exception/timer.h>
#include <exception/exception.h>
#include <common/kprint.h>
#include <common/uart.h>
#include <lib/machine.h>
#include <common/lock.h>
#include <common/types.h>
#include <common/macro.h>
#include <common/bitops.h>
#include <common/smp.h>
#include <common/tools.h>

/* Per core IRQ SOURCE MMIO address */
u64 core_irq_source[PLAT_CPU_NUM] = {
	CORE0_IRQ_SOURCE,
	CORE1_IRQ_SOURCE,
	CORE2_IRQ_SOURCE,
	CORE3_IRQ_SOURCE};

void handle_irq(int type)
{
	/**
	 * Lab 4
	 * Acquire the big kernel lock, if :
	 *	The irq is not from the kernel
	 * 	Or the thread being interrupted is an idle thread in the kernel.
	 */

	// printk("handle_irq called. cpu_id: %d\n", smp_get_cpu_id());
	if (type == IRQ_EL0_64 ||
		/* it's a irq from user type */
		current_thread->thread_ctx->type == TYPE_IDLE
		/* currently running thread is an IDLE one */)
	{
		lock_kernel();
	}

	plat_handle_irq();
	/**
	 * Lab 4
	 * Do you miss something?
	 */
	sched_handle_timer_irq();
	sched();
	// print_thread(current_thread);
	eret_to_thread(switch_context());
}

void plat_handle_irq(void)
{
	u32 cpuid = 0;
	unsigned int irq_src, irq;

	cpuid = smp_get_cpu_id();
	irq_src = get32(core_irq_source[cpuid]);

	irq = 1 << ctzl(irq_src);
	switch (irq)
	{
	case INT_SRC_TIMER3:
		handle_timer_irq();
		break;
	default:
		kinfo("Unsupported IRQ %d\n", irq);
	}
	return;
}
