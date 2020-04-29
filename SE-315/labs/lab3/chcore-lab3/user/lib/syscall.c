#include <lib/print.h>
#include <lib/syscall.h>
#include <lib/type.h>

u64 syscall(u64 sys_no, u64 arg0, u64 arg1, u64 arg2, u64 arg3, u64 arg4,
			u64 arg5, u64 arg6, u64 arg7, u64 arg8)
{

	u64 ret = 0;
	/*
	 * Lab3: Your code here
	 * Use inline assembly to store arguments into x0 to x7, store syscall number to x8,
	 * And finally use svc to execute the system call. After syscall returned, don't forget
	 * to move return value from x0 to the ret variable of this function
	 */

	volatile u64 safe_sysno = sys_no;
	volatile u64 args[9];

	args[0] = arg0;
	args[1] = arg1;
	args[2] = arg2;
	args[3] = arg3;
	args[4] = arg4;
	args[5] = arg5;
	args[6] = arg6;
	args[7] = arg7;
	args[8] = arg8;

	__asm__ volatile(
		"ldr x0, %0"
		: "=m"(args[0]));
	__asm__ volatile(
		"ldr x1, %0"
		: "=m"(args[1]));
	__asm__ volatile(
		"ldr x2, %0"
		: "=m"(args[2]));
	__asm__ volatile(
		"ldr x3, %0"
		: "=m"(args[3]));
	__asm__ volatile(
		"ldr x4, %0"
		: "=m"(args[4]));
	__asm__ volatile(
		"ldr x5, %0"
		: "=m"(args[5]));
	__asm__ volatile(
		"ldr x6, %0"
		: "=m"(args[6]));
	__asm__ volatile(
		"ldr x7, %0"
		: "=m"(args[7]));
	__asm__ volatile(
		"ldr x8, %0"
		: "=m"(safe_sysno));
	__asm__ volatile(
		"svc #1");
	__asm__("mov %0, x0"
			: "=r"(ret));
	return ret;
}

/*
 * Lab3: your code here:
 * Finish the following system calls using helper function syscall
 */
void usys_putc(char ch)
{
	syscall(SYS_putc, ch, 0, 0, 0, 0, 0, 0, 0, 0);
}

void usys_exit(int ret)
{
	syscall(SYS_exit, ret, 0, 0, 0, 0, 0, 0, 0, 0);
}

int usys_create_pmo(u64 size, u64 type)
{
	return (int)syscall(SYS_create_pmo, size, type, 0, 0, 0, 0, 0, 0, 0);
}

int usys_map_pmo(u64 process_cap, u64 pmo_cap, u64 addr, u64 rights)
{
	return (int)syscall(SYS_map_pmo, process_cap, pmo_cap, addr, rights, 0, 0, 0, 0, 0);
}

u64 usys_handle_brk(u64 addr)
{
	return syscall(SYS_handle_brk, addr, 0, 0, 0, 0, 0, 0, 0, 0);
}

/* Here finishes all syscalls need by lab3 */
