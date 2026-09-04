@ boilerplat
.syntax unified
.cpu cortex-m0plus
.thumb

.global PendSV_Handler
.extern current_task     @ tcb_t*
.extern next_task        @ tcb_t*

.section .text.PendSV_Handler
.type PendSV_Handler, %function

PendSV_Handler:
	@ disable interrupts
	CPSID	I

	@ r0 = stack top
	MRS		R0,		PSP

	@ reserve 32 bytes
	SUBS	R0,		R0,		#32

	@ r3 = stack top, r0 will be used to store registers
	MOV		R3,		R0

	@ STMIA only works with low registers (R0-R7)
	@ store real R4-7
	STMIA	R0!,	{R4-R7}

	@ store R8-11 into R4-7, use STMIA with R4-7
	MOV		R4,		R8
	MOV		R5,		R9
	MOV		R6,		R10
	MOV		R7,		R11
	STMIA	R0!,	{R4-R7}

	@ r1 = current_task (first member: *sp)
	LDR		R1,		=current_task
	LDR		R1,		[R1]
	@ save stack pointer
	STR		R3,		[R1]

	@ r2 = next_task (first member: *sp)
	LDR		R2,		=next_task
	LDR		R2,		[R2]
	@ r0 = new stack pointer
	LDR		R0,		[R2]

	@ current_task = next_task
	LDR		R1,		=current_task
	STR		R2,		[R1]

	@ r3 = new stack pointer so r0 can jump ahead and add R7-11
	MOV		R3, 	R0
	ADDS	R0,		R0,		#16
	@ handle R7-R11 first
	LDMIA	R0!,	{R4-R7}
	MOV		R8,		R4
	MOV		R9,		R5
	MOV		R10,	R6
	MOV		R11,	R7
	@ use R3 to load the real R4-R7
	LDMIA	R3!,	{R4-R7}

	@ R0 is stack top, PSP = R0
	MSR		PSP,	R0

	@ enable interrupts
	CPSIE	I

	@ return EXC_RETURN
	BX		LR

.global scheduler_start
.type scheduler_start, %function
scheduler_start:
	@ r0 = current_task
	LDR		R0,		=current_task
	LDR		R0,		[R0]
	LDR		R0,		[R0]

	@ PSP = task stack pointer
	MSR		PSP,	R0

	@ configure CONTROL
	MOVS	R0,		#2
	MSR		CONTROL,R0
	ISB

	@ trigger pendsv exception (set bit 28, PENDSVSET in ICSR)
	LDR		R0,		=0xE000ED04
	LDR		R1,		=0x10000000
	STR		R1,		[R0]

	CPSIE	I
1:  B       1b
