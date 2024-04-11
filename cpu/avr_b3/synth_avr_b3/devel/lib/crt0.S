;=============================================================================;

#if __AVR_ARCH__ == 31 || __AVR_ARCH__ == 5 || __AVR_ARCH__ == 51
	.set	VECTOR_SIZE,	4
#else
	.set	VECTOR_SIZE,	2
#endif

.set	VECTOR_SIZE,	4
.set	VECTOR_WIDTH,	4

.set	VECTOR_NUM,	1<<VECTOR_WIDTH
.set	AVR_REG_SIZE,	0x0020
.set	AVR_IO_SIZE,	0x0040
.set	RAM_SIZE, 	__DATA_REGION_LENGTH__

;=============================================================================;

.set	_RAMEND_ADDR, 	AVR_REG_SIZE + AVR_IO_SIZE + RAM_SIZE
.set	_VECTORS_SIZE,	VECTOR_SIZE * VECTOR_NUM
.set    __stack_pointer, _RAMEND_ADDR - 1
.set	AVR_STACK_POINTER_LO_ADDR, 0x3d
.set	AVR_STACK_POINTER_HI_ADDR, 0x3e
.set	AVR_SREG_ADDR, 0x3f

.macro	XJMP name
	.if (VECTOR_SIZE == 2)
	rjmp	\name
	.elseif (VECTOR_SIZE == 4)
	jmp	\name
	.else
	nop
	.endif
.endm

.macro	XCALL name
	.if (VECTOR_SIZE == 2)
	rcall	\name
	.elseif (VECTOR_SIZE == 4)
	call	\name
	.else
	nop
	.endif
.endm
	
.macro  vector name
	.if (. - __vectors < _VECTORS_SIZE)
	.weak	\name
	.set	\name, __bad_interrupt
	XJMP	\name
	.endif
.endm

.section .vectors,"ax",@progbits
	.global __vectors
	.func   __vectors

__vectors:

	XJMP	__init
	vector  __vector_1
	vector  __vector_2
	vector  __vector_3
	vector  __vector_4
	vector  __vector_5
	vector  __vector_6
	vector  __vector_7
	vector  __vector_8
	vector  __vector_9
	vector  __vector_10
	vector  __vector_11
	vector  __vector_12
	vector  __vector_13
	vector  __vector_14
	vector  __vector_15
	.endfunc

	.global __bad_interrupt
	.func   __bad_interrupt
__bad_interrupt:
	.weak   __vector_default
	.set    __vector_default, __bad_interrupt
	rjmp	__vector_default
	.endfunc

.section .init0,"ax",@progbits

__init:

.section .init2,"ax",@progbits

	eor	r1, r1
	out	AVR_SREG_ADDR, r1
	ldi     r28,lo8(__stack_pointer)
	out     AVR_STACK_POINTER_LO_ADDR, r28
	ldi     r29,hi8(__stack_pointer)
	out     AVR_STACK_POINTER_HI_ADDR, r29

.section .init9,"ax",@progbits

        XCALL   main
        XJMP    exit
