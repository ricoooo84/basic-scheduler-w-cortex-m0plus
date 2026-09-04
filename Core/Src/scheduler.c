#include "scheduler.h"

static uint32_t stack_pool[STACK_POOL_SIZE];
static uint16_t stack_index = 0;

static tcb_t all_tasks[MAX_TASKS];
static uint8_t task_count = 0;
static uint8_t task_index = 0;

static uint32_t dummy_stack[16];
static tcb_t dummy_tcb;
volatile tcb_t *current_task = NULL;
volatile tcb_t *next_task = NULL;

volatile uint32_t scheduler_tick = 0;

static tcb_t *get_next_task(void) {
	task_index = (task_index + 1)%task_count;
	return &all_tasks[task_index];
}

// void (*task_func)(void) is function address
static void task_stack_init(tcb_t *tcb) {
	// sp points to one word after end
	uint32_t *sp = tcb->stack_base + tcb->stack_size;

	// xPSR, PC, LR, R12, R3-R0
	*(--sp) = 0x01000000;
	*(--sp) = (uint32_t)tcb->function;
	*(--sp) = (uint32_t)Error_Handler;
	*(--sp) = 0x12121212;
	*(--sp) = 0x03030303;
	*(--sp) = 0x02020202;
	*(--sp) = 0x01010101;
	*(--sp) = 0x00000000;

	// R11-R4
	*(--sp) = 0x11111111;
	*(--sp) = 0x10101010;
	*(--sp) = 0x09090909;
	*(--sp) = 0x08080808;
	*(--sp) = 0x07070707;
	*(--sp) = 0x06060606;
	*(--sp) = 0x05050505;
	*(--sp) = 0x04040404;

	tcb->sp = sp;
	tcb->state = TASKSTATE_READY;
}

task_init_t scheduler_task_create(void (*task_func)(void), uint16_t words) {
	if (task_count >= MAX_TASKS) {
		return TASKINIT_MAXTASK;
	}
	if (words < 16) {
		return TASKINIT_WORDS;
	}
	if (stack_index + words > STACK_POOL_SIZE) {
		return TASKINIT_STACKOVR;
	}

	tcb_t *tcb = &all_tasks[task_count];

	tcb->state = TASKSTATE_NULL;
	tcb->function = task_func;
	tcb->stack_base = stack_pool + stack_index;
	tcb->stack_size = words;

	task_stack_init(tcb); // configures tcb->sp

	task_count++;
	stack_index += words;

	return TASKINIT_OK;
}

// call this in systick irq
void scheduler_irq(void) {
	scheduler_tick++;

	if (task_count < 2) {
		return;
	}

	// trigger PendSV_Handler
	next_task = get_next_task();
	SCB_ICSR |= PENDSVSET;
}

void scheduler_init(void) {

	if (task_count == 0) {
		Error_Handler();
	}

	current_task = &all_tasks[0];
	next_task = get_next_task();

	scheduler_start();
}
