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
	subq	$112, %rsp
	leaq	L_.str.7(%rip), %rax
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movq	%rdx, -16(%rbp)
	movq	%rax, %rdi
	movb	$0, %al
	callq	_printf
	cmpl	$4, -8(%rbp)
	movl	%eax, -72(%rbp)         ## 4-byte Spill
	jne	LBB5_8
## %bb.1:
	movq	-16(%rbp), %rax
	movq	%rax, -24(%rbp)
	movl	$0, -28(%rbp)
LBB5_2:                                 ## =>This Inner Loop Header: Depth=1
	movl	-28(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	LBB5_7
## %bb.3:                               ##   in Loop: Header=BB5_2 Depth=1
	leaq	L_.str.8(%rip), %rdi
	movq	-24(%rbp), %rax
	movq	(%rax), %rsi
	movb	$0, %al
	callq	_printf
	movl	-28(%rbp), %ecx
	movl	-4(%rbp), %edx
	subl	$1, %edx
	cmpl	%edx, %ecx
	movl	%eax, -76(%rbp)         ## 4-byte Spill
	je	LBB5_5
## %bb.4:                               ##   in Loop: Header=BB5_2 Depth=1
	leaq	L_.str.9(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -80(%rbp)         ## 4-byte Spill
LBB5_5:                                 ##   in Loop: Header=BB5_2 Depth=1
	movq	-24(%rbp), %rax
	addq	$-8, %rax
	movq	%rax, -24(%rbp)
## %bb.6:                               ##   in Loop: Header=BB5_2 Depth=1
	movl	-28(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -28(%rbp)
	jmp	LBB5_2
LBB5_7:
	jmp	LBB5_29
LBB5_8:
	cmpl	$3, -8(%rbp)
	jne	LBB5_19
## %bb.9:
	movq	-16(%rbp), %rax
	movq	%rax, -40(%rbp)
	movl	$0, -44(%rbp)
LBB5_10:                                ## =>This Inner Loop Header: Depth=1
	movl	-44(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	LBB5_18
## %bb.11:                              ##   in Loop: Header=BB5_10 Depth=1
	movq	-40(%rbp), %rax
	movq	(%rax), %rax
	movq	%rax, -56(%rbp)
	cmpq	$1, -56(%rbp)
	jne	LBB5_13
## %bb.12:                              ##   in Loop: Header=BB5_10 Depth=1
	leaq	L_.str.3(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -84(%rbp)         ## 4-byte Spill
	jmp	LBB5_14
LBB5_13:                                ##   in Loop: Header=BB5_10 Depth=1
	leaq	L_.str.4(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -88(%rbp)         ## 4-byte Spill
LBB5_14:                                ##   in Loop: Header=BB5_10 Depth=1
	movl	-44(%rbp), %eax
	movl	-4(%rbp), %ecx
	subl	$1, %ecx
	cmpl	%ecx, %eax
	je	LBB5_16
## %bb.15:                              ##   in Loop: Header=BB5_10 Depth=1
	leaq	L_.str.9(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -92(%rbp)         ## 4-byte Spill
LBB5_16:                                ##   in Loop: Header=BB5_10 Depth=1
	movq	-40(%rbp), %rax
	addq	$-8, %rax
	movq	%rax, -40(%rbp)
## %bb.17:                              ##   in Loop: Header=BB5_10 Depth=1
	movl	-44(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -44(%rbp)
	jmp	LBB5_10
LBB5_18:
	jmp	LBB5_28
LBB5_19:
	cmpl	$5, -8(%rbp)
	jne	LBB5_27
## %bb.20:
	movq	-16(%rbp), %rax
	movq	%rax, -64(%rbp)
	movl	$0, -68(%rbp)
LBB5_21:                                ## =>This Inner Loop Header: Depth=1
	movl	-68(%rbp), %eax
	cmpl	-4(%rbp), %eax
	jge	LBB5_26
## %bb.22:                              ##   in Loop: Header=BB5_21 Depth=1
	leaq	L_.str.1(%rip), %rdi
	movq	-64(%rbp), %rax
	movss	(%rax), %xmm0           ## xmm0 = mem[0],zero,zero,zero
	cvtss2sd	%xmm0, %xmm0
	movb	$1, %al
	callq	_printf
	movl	-68(%rbp), %ecx
	movl	-4(%rbp), %edx
	subl	$1, %edx
	cmpl	%edx, %ecx
	movl	%eax, -96(%rbp)         ## 4-byte Spill
	je	LBB5_24
## %bb.23:                              ##   in Loop: Header=BB5_21 Depth=1
	leaq	L_.str.9(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -100(%rbp)        ## 4-byte Spill
LBB5_24:                                ##   in Loop: Header=BB5_21 Depth=1
	movq	-64(%rbp), %rax
	addq	$-4, %rax
	movq	%rax, -64(%rbp)
## %bb.25:                              ##   in Loop: Header=BB5_21 Depth=1
	movl	-68(%rbp), %eax
	addl	$1, %eax
	movl	%eax, -68(%rbp)
	jmp	LBB5_21
LBB5_26:
	jmp	LBB5_27
LBB5_27:
	jmp	LBB5_28
LBB5_28:
	jmp	LBB5_29
LBB5_29:
	leaq	L_.str.10(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -104(%rbp)        ## 4-byte Spill
	addq	$112, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintLeftBracket   ## -- Begin function PinePrintLeftBracket
	.p2align	4, 0x90
_PinePrintLeftBracket:                  ## @PinePrintLeftBracket
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	L_.str.7(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -4(%rbp)          ## 4-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintRightBracket  ## -- Begin function PinePrintRightBracket
	.p2align	4, 0x90
_PinePrintRightBracket:                 ## @PinePrintRightBracket
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	L_.str.10(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -4(%rbp)          ## 4-byte Spill
	addq	$16, %rsp
	popq	%rbp
	retq
	.cfi_endproc
                                        ## -- End function
	.globl	_PinePrintComma         ## -- Begin function PinePrintComma
	.p2align	4, 0x90
_PinePrintComma:                        ## @PinePrintComma
	.cfi_startproc
## %bb.0:
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset %rbp, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register %rbp
	subq	$16, %rsp
	leaq	L_.str.9(%rip), %rdi
	movb	$0, %al
	callq	_printf
	movl	%eax, -4(%rbp)          ## 4-byte Spill
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

L_.str.7:                               ## @.str.7
	.asciz	"["

L_.str.8:                               ## @.str.8
	.asciz	"%lld"

L_.str.9:                               ## @.str.9
	.asciz	", "

L_.str.10:                              ## @.str.10
	.asciz	"]"


.subsections_via_symbols
