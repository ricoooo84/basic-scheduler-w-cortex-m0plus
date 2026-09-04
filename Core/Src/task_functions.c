#include "task_functions.h"

void task0(void) {
	(void)0;
}

void task1(void) {
	uint32_t last_tick = scheduler_tick;

	for (;;) {
		if (scheduler_tick - last_tick >= 1000) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_0);

			last_tick = scheduler_tick;
		}
	}
}

void task2(void) {
	uint32_t last_tick = scheduler_tick;

	for (;;) {
		if (scheduler_tick - last_tick >= 500) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_1);

			last_tick = scheduler_tick;
		}
	}
}

void task3(void) {
	uint32_t last_tick = scheduler_tick;

	for (;;) {
		if (scheduler_tick - last_tick >= 250) {
			HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_4);

			last_tick = scheduler_tick;
		}
	}
}
