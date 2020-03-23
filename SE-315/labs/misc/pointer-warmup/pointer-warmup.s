	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 12
	.globl	_foo
	.p2align	4, 0x90
_foo:                                   ## @foo
	.cfi_startproc
## BB#0:
	pushq	%rbp
Lcfi0:
	.cfi_def_cfa_offset 16
Lcfi1:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Lcfi2:
	.cfi_def_cfa_register %rbp
	subq	$112, %rsp
	leaq	L_.str(%rip), %rdi
	movl	$8, %eax
	movl	%eax, %esi
	movq	___stack_chk_guard@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rcx
	movq	%rcx, -8(%rbp)
	movb	$0, %al
	callq	_printf
	movl	$16, %edx
	movl	%edx, %edi
	movl	%eax, -72(%rbp)         ## 4-byte Spill
	callq	_malloc
	leaq	L_.str.1(%rip), %rdi
	leaq	-48(%rbp), %rsi
	movq	%rax, -56(%rbp)
	movq	-56(%rbp), %rdx
	movq	-64(%rbp), %rcx
	movb	$0, %al
	callq	_printf
	leaq	-48(%rbp), %rcx
	movq	%rcx, -64(%rbp)
	movl	$0, -68(%rbp)
	movl	%eax, -76(%rbp)         ## 4-byte Spill
LBB0_1:                                 ## =>This Inner Loop Header: Depth=1
	cmpl	$4, -68(%rbp)
	jge	LBB0_4
## BB#2:                                ##   in Loop: Header=BB0_1 Depth=1
	movl	-68(%rbp), %eax
	addl	$100, %eax
	movslq	%eax, %rcx
	movslq	-68(%rbp), %rdx
	movq	%rcx, -48(%rbp,%rdx,8)
## BB#3:                                ##   in Loop: Header=BB0_1 Depth=1
	movl	-68(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -68(%rbp)
	jmp	LBB0_1
LBB0_4:
	leaq	L_.str.2(%rip), %rdi
	movq	-64(%rbp), %rax
	movq	$200, (%rax)
	movq	-48(%rbp), %rsi
	movq	-40(%rbp), %rdx
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %r8
	movb	$0, %al
	callq	_printf
	leaq	L_.str.3(%rip), %rdi
	movq	-64(%rbp), %rcx
	movq	$300, 8(%rcx)           ## imm = 0x12C
	movq	-64(%rbp), %rcx
	movq	$301, 16(%rcx)          ## imm = 0x12D
	movq	-64(%rbp), %rcx
	movq	$302, 24(%rcx)          ## imm = 0x12E
	movq	-48(%rbp), %rsi
	movq	-40(%rbp), %rdx
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %r8
	movl	%eax, -80(%rbp)         ## 4-byte Spill
	movb	$0, %al
	callq	_printf
	leaq	L_.str.4(%rip), %rdi
	movq	-64(%rbp), %rcx
	addq	$8, %rcx
	movq	%rcx, -64(%rbp)
	movq	-64(%rbp), %rcx
	movq	$400, (%rcx)            ## imm = 0x190
	movq	-48(%rbp), %rsi
	movq	-40(%rbp), %rdx
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %r8
	movl	%eax, -84(%rbp)         ## 4-byte Spill
	movb	$0, %al
	callq	_printf
	leaq	L_.str.5(%rip), %rdi
	movq	-64(%rbp), %rcx
	addq	$1, %rcx
	movq	%rcx, -64(%rbp)
	movq	-64(%rbp), %rcx
	movq	$500, (%rcx)            ## imm = 0x1F4
	movq	-48(%rbp), %rsi
	movq	-40(%rbp), %rdx
	movq	-32(%rbp), %rcx
	movq	-24(%rbp), %r8
	movl	%eax, -88(%rbp)         ## 4-byte Spill
	movb	$0, %al
	callq	_printf
	leaq	L_.str.6(%rip), %rdi
	leaq	-48(%rbp), %rcx
	movq	%rcx, %rdx
	addq	$8, %rdx
	movq	%rdx, -56(%rbp)
	movq	%rcx, %rdx
	addq	$1, %rdx
	movq	%rdx, -64(%rbp)
	movq	-56(%rbp), %rdx
	movq	-64(%rbp), %rsi
	movq	%rsi, -96(%rbp)         ## 8-byte Spill
	movq	%rcx, %rsi
	movq	-96(%rbp), %rcx         ## 8-byte Reload
	movl	%eax, -100(%rbp)        ## 4-byte Spill
	movb	$0, %al
	callq	_printf
	movq	___stack_chk_guard@GOTPCREL(%rip), %rcx
	movq	(%rcx), %rcx
	movq	-8(%rbp), %rdx
	cmpq	%rdx, %rcx
	movl	%eax, -104(%rbp)        ## 4-byte Spill
	jne	LBB0_6
## BB#5:
	addq	$112, %rsp
	popq	%rbp
	retq
LBB0_6:
	callq	___stack_chk_fail
	.cfi_endproc

	.globl	_main
	.p2align	4, 0x90
_main:                                  ## @main
	.cfi_startproc
## BB#0:
	pushq	%rbp
Lcfi3:
	.cfi_def_cfa_offset 16
Lcfi4:
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
Lcfi5:
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	L_.str.7(%rip), %rdi
	movl	$0, -4(%rbp)
	movb	$0, %al
	callq	_printf
	leaq	L_.str.8(%rip), %rdi
	movl	%eax, -8(%rbp)          ## 4-byte Spill
	movb	$0, %al
	callq	_printf
	movl	%eax, -12(%rbp)         ## 4-byte Spill
	callq	_foo
	xorl	%eax, %eax
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc

	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"sizeof long: %lu\n"

L_.str.1:                               ## @.str.1
	.asciz	"1: a = %p, b = %p, c = %p\n"

L_.str.2:                               ## @.str.2
	.asciz	"2: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n"

L_.str.3:                               ## @.str.3
	.asciz	"3: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n"

L_.str.4:                               ## @.str.4
	.asciz	"4: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n"

L_.str.5:                               ## @.str.5
	.asciz	"5: a[0] = %ld, a[1] = %ld, a[2] = %ld, a[3] = %ld\n"

L_.str.6:                               ## @.str.6
	.asciz	"6: a = %p, b = %p, c = %p\n"

L_.str.7:                               ## @.str.7
	.asciz	">>> An basic pointer exercies for ChCore.\n"

L_.str.8:                               ## @.str.8
	.asciz	">>> Please ensure you understand these output :)\n"


.subsections_via_symbols
