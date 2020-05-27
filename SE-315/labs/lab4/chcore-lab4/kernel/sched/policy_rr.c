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
/* Scheduler related functions are implemented here */
#include <sched/sched.h>
#include <common/smp.h>
#include <common/kprint.h>
#include <lib/machine.h>
#include <common/kmalloc.h>
#include <common/list.h>
#include <common/util.h>
#include <process/thread.h>
#include <common/macro.h>
#include <lib/errno.h>
#include <process/thread.h>
#include <exception/irq.h>
#include <sched/context.h>
#include <exception/exception.h>
#include <common/lock.h>

/* in arch/sched/idle.S */
void idle_thread_routine(void);

/*
 * rr_ready_queue
 * Per-CPU ready queue for ready tasks.
 * this struct list_head sucks, so let's just throw it away.
 */

// this is a dummy queue, don't use that one
struct list_head rr_ready_queue[PLAT_CPU_NUM];

static struct thread *priv_ready_queue[PLAT_CPU_NUM][MAX_PRIO];
static u32 queue_pointer[PLAT_CPU_NUM];
struct lock mutex_lock[PLAT_CPU_NUM];

#if 0
void printkk(const char *fmt)
{
	if (smp_get_cpu_id() == 0)
	{
		printk(fmt);
	}
}

static void print_queue()
{
	if (smp_get_cpu_id() != 0)
	{
		return;
	}

	printk("==============================\n");
	for (size_t i = 0; i < PLAT_CPU_NUM; i++)
	{
		printk("#%d\t", i);
		lock(&mutex_lock[i]);
		for (u32 j = 0; j < queue_pointer[i]; j++)
		{
			print_thread(priv_ready_queue[i][j]);
		}
		unlock(&mutex_lock[i]);
		printk("\n");
	}
	printk("==============================\n");
}
#else
#define printkk(...)
static void print_queue()
{
	// do nothing
}
#endif

static bool queue_empty(u32 cpu_id)
{
	return queue_pointer[cpu_id] == 0;
}

static void set_dummy_queue(int empty, u32 cpu_id)
{
	lock(&mutex_lock[cpu_id]);
	if (empty)
	{
		rr_ready_queue[cpu_id].next = rr_ready_queue[cpu_id].prev = &rr_ready_queue[cpu_id];
		BUG_ON(!list_empty(&rr_ready_queue[cpu_id]));
	}
	else
	{
		rr_ready_queue[cpu_id].next = (struct list_head *)0xdead;
		rr_ready_queue[cpu_id].prev = (struct list_head *)0xbeef;
		BUG_ON(list_empty(&rr_ready_queue[cpu_id]));
	}
	unlock(&mutex_lock[cpu_id]);
}

static void refresh_dummy_queue()
{
	for (u32 i = 0; i < PLAT_CPU_NUM; i++)
	{
		set_dummy_queue(queue_empty(i), i);
	}
}

static void init_queue(u32 cpu_count)
{
	print_queue();
	for (u32 i = 0; i < cpu_count; i++)
	{
		lock(&mutex_lock[i]);
		queue_pointer[i] = 0;
		unlock(&mutex_lock[i]);
	}
	refresh_dummy_queue();
}

static void enqueue_last(struct thread *thread, u32 cpu_id)
{
	print_queue();
	lock(&mutex_lock[cpu_id]);
	priv_ready_queue[cpu_id][queue_pointer[cpu_id]] = thread;
	++queue_pointer[cpu_id];
	unlock(&mutex_lock[cpu_id]);
	refresh_dummy_queue();
}

/*
static struct thread *dequeue_first(struct thread *thread, u32 cpu_id)
{
	print_queue();
	BUG_ON(queue_pointer[cpu_id] == 0);

	if (thread == NULL)
	{
		lock(&mutex_lock[cpu_id]);
		// DQ the first one, by default
		struct thread *result = priv_ready_queue[cpu_id][0];

		for (u32 i = 1; i < queue_pointer[cpu_id]; i++)
		{
			priv_ready_queue[cpu_id][i - 1] = priv_ready_queue[cpu_id][i];
		}
		--queue_pointer[cpu_id];
		unlock(&mutex_lock[cpu_id]);
		refresh_dummy_queue();
		return result;
	}
	else
	{
		lock(&mutex_lock[cpu_id]);
		struct thread *new_queue[MAX_PRIO];
		int new_pointer = 0;

		int found = false;
		struct thread *result = NULL;

		for (u32 i = 0; i < queue_pointer[cpu_id]; i++)
		{
			if (priv_ready_queue[cpu_id][i] != thread)
			{
				new_queue[new_pointer] = priv_ready_queue[cpu_id][i];
				++new_pointer;
			}
			else if (found)
			{
				new_queue[new_pointer] = priv_ready_queue[cpu_id][i];
				++new_pointer;
			}
			else
			{
				found = true;
				result = priv_ready_queue[cpu_id][i];
			}
		}

		for (u32 j = 0; j < new_pointer; j++)
		{
			priv_ready_queue[cpu_id][j] = new_queue[j];
		}
		queue_pointer[cpu_id] = new_pointer;

		unlock(&mutex_lock[cpu_id]);
		refresh_dummy_queue();
		return result;
	}
}
*/

static void dequeue_any(struct thread *thread, u32 cpu_id)
{
	print_queue();
	lock(&mutex_lock[cpu_id]);
	struct thread *new_queue[MAX_PRIO];
	int new_pointer = 0;

	for (u32 i = 0; i < queue_pointer[cpu_id]; i++)
	{
		if (priv_ready_queue[cpu_id][i] != thread)
		{
			new_queue[new_pointer] = priv_ready_queue[cpu_id][i];
			++new_pointer;
		}
	}

	for (u32 j = 0; j < new_pointer; j++)
	{
		priv_ready_queue[cpu_id][j] = new_queue[j];
	}
	queue_pointer[cpu_id] = new_pointer;
	unlock(&mutex_lock[cpu_id]);
	refresh_dummy_queue();
}

// struct list_head rr_ready_queue[PLAT_CPU_NUM];
/*
 * RR policy also has idle threads.
 * When no active user threads in ready queue,
 * we will choose the idle thread to execute.
 * Idle thread will **NOT** be in the RQ.
 */
struct thread idle_threads[PLAT_CPU_NUM];

int rr_sched_enqueue(struct thread *thread);
int rr_sched_dequeue(struct thread *thread);

/*
 * Lab 4
 * Sched_enqueue
 * Put `thread` at the end of ready queue of assigned `affinity`.
 * If affinity = NO_AFF, assign the core to the current cpu.
 * If the thread is IDLE thread, do nothing!
 * Do not forget to check if the affinity is valid!
 */
int rr_sched_enqueue(struct thread *thread)
{
	// printkk("attempt to enqueue this: \n\t");
	// print_thread(thread);
	if (!thread || !thread->thread_ctx)
	{
		return -1;
	}

	// thread->node.thread = thread;

	u32 affinity = thread->thread_ctx->affinity;
	if (thread->thread_ctx->type == TYPE_IDLE)
	{
		// idle thread? ignore it.
		return 0;
	}
	else if (thread->thread_ctx->state == TS_READY)
	{
		// it's already an enqueued thread!
		printkk("going to return -3! bad state\n");
		return -3;
	}

	if (affinity == NO_AFF)
	{
		// printk("NO AFF\n");
		u32 cpu_id = smp_get_cpu_id();

		enqueue_last(thread, cpu_id);
		// list_add(&thread->node, &rr_ready_queue[cpu_id]);
		// printk("perform list_add.\n");
		// print_thread(thread);
		thread->thread_ctx->cpuid = cpu_id;
	}
	else if (affinity < PLAT_CPU_NUM)
	{
		// printk("HAVE AFF %d\n", affinity);
		enqueue_last(thread, affinity);
		// list_add(&thread->node, &rr_ready_queue[affinity]);
		// printk("perform list_add.\n");
		// print_thread(thread);
		thread->thread_ctx->cpuid = affinity;
	}
	else
	{
		// printk("BUG: bad affinity given to rr_sched_enqueue!\n");
		printkk("going to return -4! bad affinity\n");
		return -4;
	}
	thread->thread_ctx->state = TS_READY;
	// print_thread(thread);
	return 0;
}

/*
 * Lab 4
 * Sched_dequeue
 * remove `thread` from its current residual ready queue
 * Do not forget to add some basic parameter checking
 */
int rr_sched_dequeue(struct thread *thread)
{
	// printk("rr_sched_dequeue called with:\n");
	// print_thread(thread);
	if (!thread || !thread->thread_ctx)
	{
		return -1;
	}

	if (thread->thread_ctx->type == TYPE_IDLE || thread->thread_ctx->state != TS_READY)
	{
		// reject dequeuing an idle thread
		return -2;
	}

	thread->thread_ctx->state = TS_INTER;

	u32 cpu_id = thread->thread_ctx->cpuid;
	dequeue_any(thread, cpu_id);
	// struct list_head *head = &rr_ready_queue[cpu_id];
	// struct list_head *node = head;

	// do
	// {
	// 	struct list_head *prev = node->prev;
	// 	if (node->thread == thread)
	// 	{
	// 		// printk("going to delete %p\n", node);
	// 		list_del(node);
	// 		break;
	// 	}
	// 	// printk("in while loop, node = %p, prev = %p, head = %p\n", node, prev, head);
	// 	node = prev;
	// } while (node && head != node);
	return 0;
}

/*
 * Lab 4
 * The helper function
 * Choose an appropriate thread and dequeue from ready queue
 * 
 * If there is no ready thread in the current CPU's ready queue, 
 * choose the idle thread of the CPU.
 * 
 * Do not forget to check the type and 
 * state of the chosen thread
 */
struct thread *rr_sched_choose_thread(void)
{
	// printkk("called rr_sched_choose_thread\n");
	u32 cpu_id = smp_get_cpu_id();

	if (queue_empty(cpu_id))
	{
		// if (list_empty(&rr_ready_queue[cpu_id])) {
		// printkk("list is empty!\n");
		return &idle_threads[cpu_id];
	}
	// printkk("list isn't empty!\n");

	for (u32 i = 0; i < queue_pointer[cpu_id]; i++)
	{
		struct thread *target = priv_ready_queue[cpu_id][i];
		if (target && target->thread_ctx && target->thread_ctx->cpuid == cpu_id && target->thread_ctx->state == TS_READY && target->thread_ctx->type != TYPE_IDLE)
		{
			// print_thread(target);
			rr_sched_dequeue(target);
			return target;
		}
		// printk("this cycle done in vain\n");
		// node = node->prev;
	}

	// printk(" === traverse over === \n");
	return &idle_threads[cpu_id];
}

/*
 * Lab 4
 * Schedule a thread to execute.
 * This function will suspend current running thread, if any, and schedule
 * another thread from `rr_ready_queue[cpu_id]`.
 * 
 * Hints:
 * Macro DEFAULT_BUDGET defines the value for resetting thread's budget.
 * After you get one thread from rr_sched_choose_thread, pass it to
 * switch_to_thread() to prepare for switch_context().
 * Then ChCore can call eret_to_thread() to return to user mode.
 */
int rr_sched(void)
{
	// printk("rr_sched called\n");
	if (current_thread && current_thread->thread_ctx && current_thread->thread_ctx->type != TYPE_IDLE && current_thread->thread_ctx->state == TS_RUNNING)
	{
		if (current_thread->thread_ctx->sc->budget != 0)
		{
			// no reschedule if budget != 0
			return 0;
		}
		rr_sched_enqueue(current_thread);
	}
	// printkk("called rr_sched\n");
	struct thread *target = rr_sched_choose_thread();
	// printkk("get a chosen target\n");
	// print_thread(target);

	target->thread_ctx->sc->budget = DEFAULT_BUDGET;

	// printkk("enqueue over\n");

	switch_to_thread(target);
	// kinfo("ok till then...\n");
	return 0;
}

/*
 * Initialize the per thread queue and idle thread.
 */
int rr_sched_init(void)
{
	// printk("called rr_sched_init\n");
	int i = 0;

	/* Initialize global variables */
	for (i = 0; i < PLAT_CPU_NUM; i++)
	{
		current_threads[i] = NULL;
		// init_list_head(&rr_ready_queue[i]);
		// rr_ready_queue[i].thread = NULL;
		lock_init(&mutex_lock[i]);
	}

	init_queue(PLAT_CPU_NUM);
	refresh_dummy_queue();

	print_queue();

	/* Initialize one idle thread for each core and insert into the RQ */
	for (i = 0; i < PLAT_CPU_NUM; i++)
	{
		/* Set the thread context of the idle threads */
		BUG_ON(!(idle_threads[i].thread_ctx = create_thread_ctx()));
		/* We will set the stack and func ptr in arch_idle_ctx_init */
		init_thread_ctx(&idle_threads[i], 0, 0, MIN_PRIO, TYPE_IDLE, i);
		/* Call arch-dependent function to fill the context of the idle
		 * threads */
		arch_idle_ctx_init(idle_threads[i].thread_ctx,
						   idle_thread_routine);
		/* Idle thread is kernel thread which do not have vmspace */
		// idle_threads[i].node.thread = &idle_threads[i];
		idle_threads[i].vmspace = NULL;
	}
	kdebug("Scheduler initialized. Create %d idle threads.\n", i);
	return 0;
}

/*
 * Lab 4
 * Handler called each time a timer interrupt is handled
 * Do not forget to call sched_handle_timer_irq() in proper code location.
 */
void rr_sched_handle_timer_irq(int force)
{
	// printk("timer tick with %d\n", force);
	// printk("timer ticked. force? %d\n", force);
	if (force)
	{
		current_thread->thread_ctx->sc->budget = 0;
	}
	else if (current_thread->thread_ctx->sc->budget != 0)
	{
		current_thread->thread_ctx->sc->budget -= 1;
	}
}

struct sched_ops rr = {
	.sched_init = rr_sched_init,
	.sched = rr_sched,
	.sched_enqueue = rr_sched_enqueue,
	.sched_dequeue = rr_sched_dequeue,
	.sched_choose_thread = rr_sched_choose_thread,
	.sched_handle_timer_irq = rr_sched_handle_timer_irq,
};
