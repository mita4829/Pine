	.section	__TEXT,__text,regular,pure_instructions
	.macosx_version_min 10, 13
	.globl	_PinePrint              ## -- Begin function PinePrint
	.p2align	4, 0x90
_PinePrint:                             ## @PinePrint
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
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	cmpl	$4, -4(%rbp)
	jne	LBB0_2
## BB#1:
	leaq	L_.str(%rip), %rdi
	movq	-16(%rbp), %rax
	movl	(%rax), %esi
	movb	$0, %al
	callq	_printf
	movl	%eax, -20(%rbp)         ## 4-byte Spill
	jmp	LBB0_12
LBB0_2:
	cmpl	$5, -4(%rbp)
	jne	LBB0_4
## BB#3:
	leaq	L_.str.1(%rip), %rdi
	movq	-16(%rbp), %rax
	cvtss2sd	(%rax), %xmm0
	movb	$1, %al
	callq	_printf
	movl	%eax, -24(%rbp)         ## 4-byte Spill
	jmp	LBB0_12
LBB0_4:
	cmpl	$6, -4(%rbp)
	jne	LBB0_6
## BB#5:
	leaq	L_.str.2(%rip), %rdi
	movq	-16(%rbp), %rax
	movsd	(%rax), %xmm0           ## xmm0 = mem[0],zero
	movb	$1, %al
	callq	_printf
	movl	%eax, -28(%rbp)         ## 4-byte Spill
	jmp	LBB0_12
LBB0_6:
	cmpl	$7, -4(%rbp)
	jne	LBB0_8
## BB#7:
	leaq	L_.str.3(%rip), %rdi
	movq	-16(%rbp), %rsi
	movb	$0, %al
	callq	_printf
	movl	%eax, -32(%rbp)         ## 4-byte Spill
	jmp	LBB0_12
LBB0_8:
	jmp	LBB0_9
LBB0_9:
	jmp	LBB0_10
LBB0_10:
	jmp	LBB0_11
LBB0_11:
	leaq	L_.str.4(%rip), %rdi
	movl	-4(%rbp), %esi
	movb	$0, %al
	callq	_printf
	movl	%eax, -36(%rbp)         ## 4-byte Spill
LBB0_12:
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%d\n"

L_.str.1:                               ## @.str.1
	.asciz	"%f\n"

L_.str.2:                               ## @.str.2
	.asciz	"%lf\n"

L_.str.3:                               ## @.str.3
	.asciz	"%s\n"

L_.str.4:                               ## @.str.4
	.asciz	"Unknown id: %d\n"


.subsections_via_symbols
