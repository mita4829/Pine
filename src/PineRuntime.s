	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 10, 14
	.globl	_PinePrint              ## -- Begin function PinePrint
	.p2align	4, 0x90
_PinePrint:                             ## @PinePrint
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$48, %rsp
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	cmpl	$4, -4(%rbp)
	jne	LBB0_2
## %bb.1:
	leaq	L_.str(%rip), %rdi
	movq	-16(%rbp), %rax
	movq	(%rax), %rsi
	movb	$0, %al
	callq	_printf
	movl	%eax, -24(%rbp)         ## 4-byte Spill
	jmp	LBB0_18
LBB0_2:
	cmpl	$5, -4(%rbp)
	jne	LBB0_4
## %bb.3:
	leaq	L_.str.1(%rip), %rdi
	movq	-16(%rbp), %rax
	movsd	(%rax), %xmm0           ## xmm0 = mem[0],zero
	movb	$1, %al
	callq	_printf
	movl	%eax, -28(%rbp)         ## 4-byte Spill
	jmp	LBB0_18
LBB0_4:
	cmpl	$6, -4(%rbp)
	jne	LBB0_6
## %bb.5:
	leaq	L_.str.1(%rip), %rdi
	movq	-16(%rbp), %rax
	movsd	(%rax), %xmm0           ## xmm0 = mem[0],zero
	movb	$1, %al
	callq	_printf
	movl	%eax, -32(%rbp)         ## 4-byte Spill
	jmp	LBB0_18
LBB0_6:
	cmpl	$3, -4(%rbp)
	jne	LBB0_11
## %bb.7:
	movq	-16(%rbp), %rax
	movl	(%rax), %ecx
	movl	%ecx, -20(%rbp)
	cmpl	$1, -20(%rbp)
	jne	LBB0_9
## %bb.8:
	leaq	L_.str.2(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -36(%rbp)         ## 4-byte Spill
	jmp	LBB0_10
LBB0_9:
	leaq	L_.str.3(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -40(%rbp)         ## 4-byte Spill
LBB0_10:
	jmp	LBB0_18
LBB0_11:
	cmpl	$7, -4(%rbp)
	jne	LBB0_13
## %bb.12:
	leaq	L_.str.4(%rip), %rdi
	movq	-16(%rbp), %rax
	movq	(%rax), %rsi
	movb	$0, %al
	callq	_printf
	movl	%eax, -44(%rbp)         ## 4-byte Spill
	jmp	LBB0_18
LBB0_13:
	jmp	LBB0_14
LBB0_14:
	jmp	LBB0_15
LBB0_15:
	jmp	LBB0_16
LBB0_16:
	jmp	LBB0_17
LBB0_17:
	leaq	L_.str.5(%rip), %rdi
	movl	-4(%rbp), %esi
	movb	$0, %al
	callq	_printf
	movl	%eax, -48(%rbp)         ## 4-byte Spill
LBB0_18:
	addq	$48, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintInt           ## -- Begin function PinePrintInt
	.p2align	4, 0x90
_PinePrintInt:                          ## @PinePrintInt
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	L_.str.6(%rip), %rax
	movl	%edi, -4(%rbp)
	movl	-4(%rbp), %esi
	movq	%rax, %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -8(%rbp)          ## 4-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintString        ## -- Begin function PinePrintString
	.p2align	4, 0x90
_PinePrintString:                       ## @PinePrintString
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	L_.str.4(%rip), %rax
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rsi
	movq	%rax, %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -12(%rbp)         ## 4-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintBoolean       ## -- Begin function PinePrintBoolean
	.p2align	4, 0x90
_PinePrintBoolean:                      ## @PinePrintBoolean
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	cmpl	$0, -4(%rbp)
	je	LBB3_2
## %bb.1:
	leaq	L_.str.2(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -8(%rbp)          ## 4-byte Spill
	jmp	LBB3_3
LBB3_2:
	leaq	L_.str.3(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -12(%rbp)         ## 4-byte Spill
LBB3_3:
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintFloat         ## -- Begin function PinePrintFloat
	.p2align	4, 0x90
_PinePrintFloat:                        ## @PinePrintFloat
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	L_.str.1(%rip), %rdi
	movsd	%xmm0, -8(%rbp)
	movsd	-8(%rbp), %xmm0         ## xmm0 = mem[0],zero
	movb	$1, %al
	callq	_printf
	movl	%eax, -12(%rbp)         ## 4-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintDouble        ## -- Begin function PinePrintDouble
	.p2align	4, 0x90
_PinePrintDouble:                       ## @PinePrintDouble
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	L_.str.1(%rip), %rdi
	movsd	%xmm0, -8(%rbp)
	movsd	-8(%rbp), %xmm0         ## xmm0 = mem[0],zero
	movb	$1, %al
	callq	_printf
	movl	%eax, -12(%rbp)         ## 4-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintArrayHelper   ## -- Begin function PinePrintArrayHelper
	.p2align	4, 0x90
_PinePrintArrayHelper:                  ## @PinePrintArrayHelper
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movq	%rdx, -16(%rbp)
	movl	$0, -20(%rbp)
LBB6_1:                                 ## =>This Inner Loop Header: Depth=1
	movl	-20(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jge	LBB6_24
## %bb.2:                               ##   in Loop: Header=BB6_1 Depth=1
	cmpl	$4, -4(%rbp)
	jne	LBB6_4
## %bb.3:                               ##   in Loop: Header=BB6_1 Depth=1
	leaq	L_.str(%rip), %rdi
	movq	-16(%rbp), %rax
	movq	(%rax), %rsi
	movb	$0, %al
	callq	_printf
	movl	%eax, -36(%rbp)         ## 4-byte Spill
	jmp	LBB6_20
LBB6_4:                                 ##   in Loop: Header=BB6_1 Depth=1
	cmpl	$5, -4(%rbp)
	jne	LBB6_6
## %bb.5:                               ##   in Loop: Header=BB6_1 Depth=1
	leaq	L_.str.1(%rip), %rdi
	movq	-16(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -32(%rbp)
	movsd	-32(%rbp), %xmm0        ## xmm0 = mem[0],zero
	movb	$1, %al
	callq	_printf
	movl	%eax, -40(%rbp)         ## 4-byte Spill
	jmp	LBB6_19
LBB6_6:                                 ##   in Loop: Header=BB6_1 Depth=1
	cmpl	$3, -4(%rbp)
	jne	LBB6_11
## %bb.7:                               ##   in Loop: Header=BB6_1 Depth=1
	movq	-16(%rbp), %rax
	cmpq	$0, (%rax)
	je	LBB6_9
## %bb.8:                               ##   in Loop: Header=BB6_1 Depth=1
	leaq	L_.str.2(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -44(%rbp)         ## 4-byte Spill
	jmp	LBB6_10
LBB6_9:                                 ##   in Loop: Header=BB6_1 Depth=1
	leaq	L_.str.3(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -48(%rbp)         ## 4-byte Spill
LBB6_10:                                ##   in Loop: Header=BB6_1 Depth=1
	jmp	LBB6_18
LBB6_11:                                ##   in Loop: Header=BB6_1 Depth=1
	cmpl	$7, -4(%rbp)
	jne	LBB6_13
## %bb.12:                              ##   in Loop: Header=BB6_1 Depth=1
	leaq	L_.str.7(%rip), %rdi
	movq	-16(%rbp), %rax
	movq	(%rax), %rsi
	movb	$0, %al
	callq	_printf
	movl	%eax, -52(%rbp)         ## 4-byte Spill
	jmp	LBB6_17
LBB6_13:                                ##   in Loop: Header=BB6_1 Depth=1
	cmpl	$6, -4(%rbp)
	jne	LBB6_15
## %bb.14:                              ##   in Loop: Header=BB6_1 Depth=1
	leaq	L_.str.1(%rip), %rdi
	movq	-16(%rbp), %rax
	movq	(%rax), %rax
	cvtsi2sdq	%rax, %xmm0
	movb	$1, %al
	callq	_printf
	movl	%eax, -56(%rbp)         ## 4-byte Spill
	jmp	LBB6_16
LBB6_15:                                ##   in Loop: Header=BB6_1 Depth=1
	leaq	L_.str.8(%rip), %rdi
	movl	-4(%rbp), %esi
	movb	$0, %al
	callq	_printf
	movl	%eax, -60(%rbp)         ## 4-byte Spill
LBB6_16:                                ##   in Loop: Header=BB6_1 Depth=1
	jmp	LBB6_17
LBB6_17:                                ##   in Loop: Header=BB6_1 Depth=1
	jmp	LBB6_18
LBB6_18:                                ##   in Loop: Header=BB6_1 Depth=1
	jmp	LBB6_19
LBB6_19:                                ##   in Loop: Header=BB6_1 Depth=1
	jmp	LBB6_20
LBB6_20:                                ##   in Loop: Header=BB6_1 Depth=1
	movq	-16(%rbp), %rax
	addq	$-8, %rax
	movq	%rax, -16(%rbp)
	movl	-20(%rbp), %ecx
	movl	-8(%rbp), %edx
	subl	$1, %edx
	cmpl	%edx, %ecx
	je	LBB6_22
## %bb.21:                              ##   in Loop: Header=BB6_1 Depth=1
	leaq	L_.str.9(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -64(%rbp)         ## 4-byte Spill
LBB6_22:                                ##   in Loop: Header=BB6_1 Depth=1
	jmp	LBB6_23
LBB6_23:                                ##   in Loop: Header=BB6_1 Depth=1
	movl	-20(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -20(%rbp)
	jmp	LBB6_1
LBB6_24:
	movq	-16(%rbp), %rax
	addq	$64, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintArray         ## -- Begin function PinePrintArray
	.p2align	4, 0x90
_PinePrintArray:                        ## @PinePrintArray
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$64, %rsp
	leaq	L_.str.10(%rip), %rax
	movl	$4294967295, %ecx       ## imm = 0xFFFFFFFF
	movl	%ecx, %edx
	movl	%edi, -4(%rbp)
	movq	%rsi, -16(%rbp)
	movq	-16(%rbp), %rsi
	movq	%rsi, -24(%rbp)
	movq	-24(%rbp), %rsi
	andq	(%rsi), %rdx
	movq	%rdx, -32(%rbp)
	movq	-24(%rbp), %rdx
	movq	(%rdx), %rdx
	shrq	$32, %rdx
	movq	%rdx, -40(%rbp)
	movq	%rax, %rdi
	movb	$0, %al
	callq	_printf
	cmpq	$1, -40(%rbp)
	movl	%eax, -56(%rbp)         ## 4-byte Spill
	jbe	LBB7_8
## %bb.1:
	movq	-24(%rbp), %rax
	addq	$-8, %rax
	movq	%rax, -24(%rbp)
	movl	$0, -52(%rbp)
LBB7_2:                                 ## =>This Inner Loop Header: Depth=1
	movslq	-52(%rbp), %rax
	cmpq	-32(%rbp), %rax
	jae	LBB7_7
## %bb.3:                               ##   in Loop: Header=BB7_2 Depth=1
	movl	-4(%rbp), %edi
	movq	-24(%rbp), %rax
	movq	%rax, %rsi
	callq	_PinePrintArray
	movq	%rax, -24(%rbp)
	movslq	-52(%rbp), %rax
	movq	-32(%rbp), %rsi
	subq	$1, %rsi
	cmpq	%rsi, %rax
	je	LBB7_5
## %bb.4:                               ##   in Loop: Header=BB7_2 Depth=1
	leaq	L_.str.9(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -60(%rbp)         ## 4-byte Spill
LBB7_5:                                 ##   in Loop: Header=BB7_2 Depth=1
	jmp	LBB7_6
LBB7_6:                                 ##   in Loop: Header=BB7_2 Depth=1
	movl	-52(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -52(%rbp)
	jmp	LBB7_2
LBB7_7:
	jmp	LBB7_9
LBB7_8:
	movq	-24(%rbp), %rax
	addq	$-8, %rax
	movq	%rax, -24(%rbp)
	movl	-4(%rbp), %edi
	movq	-32(%rbp), %rax
	movl	%eax, %ecx
	movq	-24(%rbp), %rdx
	movl	%ecx, %esi
	callq	_PinePrintArrayHelper
	movq	%rax, -24(%rbp)
LBB7_9:
	leaq	L_.str.11(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movq	-24(%rbp), %rdi
	movl	%eax, -64(%rbp)         ## 4-byte Spill
	movq	%rdi, %rax
	addq	$64, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%lld"

L_.str.1:                               ## @.str.1
	.asciz	"%lf"

L_.str.2:                               ## @.str.2
	.asciz	"True"

L_.str.3:                               ## @.str.3
	.asciz	"False"

L_.str.4:                               ## @.str.4
	.asciz	"%s"

L_.str.5:                               ## @.str.5
	.asciz	"Unknown id: %d\n"

L_.str.6:                               ## @.str.6
	.asciz	"%d"

L_.str.7:                               ## @.str.7
	.asciz	"\"%s\""

L_.str.8:                               ## @.str.8
	.asciz	"Unknown type: %d\n"

L_.str.9:                               ## @.str.9
	.asciz	", "

L_.str.10:                              ## @.str.10
	.asciz	"["

L_.str.11:                              ## @.str.11
	.asciz	"]"


.subsections_via_symbols
