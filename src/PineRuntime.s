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
	movl	(%rax), %esi
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
	movss	(%rax), %xmm0           ## xmm0 = mem[0],zero,zero,zero
	cvtss2sd	%xmm0, %xmm0
	movb	$1, %al
	callq	_printf
	movl	%eax, -28(%rbp)         ## 4-byte Spill
	jmp	LBB0_18
LBB0_4:
	cmpl	$6, -4(%rbp)
	jne	LBB0_6
## %bb.5:
	leaq	L_.str.2(%rip), %rdi
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
	leaq	L_.str.3(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -36(%rbp)         ## 4-byte Spill
	jmp	LBB0_10
LBB0_9:
	leaq	L_.str.4(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -40(%rbp)         ## 4-byte Spill
LBB0_10:
	jmp	LBB0_18
LBB0_11:
	cmpl	$7, -4(%rbp)
	jne	LBB0_13
## %bb.12:
	leaq	L_.str.5(%rip), %rdi
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
	leaq	L_.str.6(%rip), %rdi
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
	leaq	L_.str(%rip), %rax
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
	leaq	L_.str.5(%rip), %rax
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
	leaq	L_.str.3(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -8(%rbp)          ## 4-byte Spill
	jmp	LBB3_3
LBB3_2:
	leaq	L_.str.4(%rip), %rdi
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
	movss	%xmm0, -4(%rbp)
	movss	-4(%rbp), %xmm0         ## xmm0 = mem[0],zero,zero,zero
	cvtss2sd	%xmm0, %xmm0
	movb	$1, %al
	callq	_printf
	movl	%eax, -8(%rbp)          ## 4-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.section	__TEXT,__cstring,cstring_literals
L_.str:                                 ## @.str
	.asciz	"%d"

L_.str.1:                               ## @.str.1
	.asciz	"%f"

L_.str.2:                               ## @.str.2
	.asciz	"%lf"

L_.str.3:                               ## @.str.3
	.asciz	"True"

L_.str.4:                               ## @.str.4
	.asciz	"False"

L_.str.5:                               ## @.str.5
	.asciz	"%s"

L_.str.6:                               ## @.str.6
	.asciz	"Unknown id: %d\n"


.subsections_via_symbols
