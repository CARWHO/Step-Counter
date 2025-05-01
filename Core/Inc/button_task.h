#ifndef BUTTON_TASK_H
#define BUTTON_TASK_H

#include <stdio.h>
#include "buttons.h"  // Ensure this header defines buttonState_t

void uart_transmit_button(buttonState_t downState);
void button_task_init(void);
void button_task_execute(void);

#define NUM_BUT_POLLS_1 3
#define NUM_BUT_POLLS_2 3
#define NUM_BUT_POLLS_3 3
#define NUM_BUT_POLLS_4 3

#endif
