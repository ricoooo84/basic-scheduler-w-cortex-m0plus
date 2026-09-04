#ifndef INC_SCHEDULER_H_
#define INC_SCHEDULER_H_

#include <stdint.h>
#include "main.h"

#define	SCB_ICSR		(*(volatile uint32_t *)0xE000ED04)
#define PENDSVSET		(1UL << 28)

#define EXC_RETURN		0xFFFFFFFD

#define MAX_TASKS		20
// words
#define STACK_POOL_SIZE	2048 // has to be even (ensure 8-byte aligned)

extern volatile uint32_t scheduler_tick;
extern void scheduler_start(void);

typedef enum {
	TASKSTATE_READY,
	TASKSTATE_NULL,
	TASKSTATE_RUNNING,
	TASKSTATE_BLOCKED
} task_state_t;

typedef enum {
	TASKINIT_OK,
	TASKINIT_WORDS,
	TASKINIT_MAXTASK,
	TASKINIT_STACKOVR,
} task_init_t;

typedef struct {
	uint32_t *sp;
	uint32_t *stack_base;
	void (*function)(void);
	task_state_t state;

	uint16_t stack_size;
} tcb_t;

task_init_t scheduler_task_create(void (*task_func)(void), uint16_t words);
void scheduler_irq(void);
void scheduler_init(void);


#endif /* INC_SCHEDULER_H_ */
