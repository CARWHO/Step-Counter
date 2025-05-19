#ifndef BUTTON_TASK_H
#define BUTTON_TASK_H

#include <stdio.h>
#include "buttons.h"  // Ensure this header defines buttonState_t


// transmitting data over serial when SW1 pressed
void uart_transmit_button(buttonState_t downState);

// Initialising function for the module
void button_task_init(void);

// Executing function for the module
void button_task_execute(void);

// Getting state SW1 to detect for button press
uint8_t UART_button_state_get(void);

#define NUM_BUT_POLLS_1 3
#define NUM_BUT_POLLS_2 3
#define NUM_BUT_POLLS_3 3
#define NUM_BUT_POLLS_4 3

#endif
