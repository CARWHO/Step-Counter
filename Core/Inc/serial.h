/*
 * serial.h
 *
 *  Created on: May 6, 2025
 *      Author: una14
 */

#ifndef INC_SERIAL_H_
#define INC_SERIAL_H_

#include <stdio.h>
#include "buttons.h"

#define UART_BUFFER_SIZE 100

extern char uart_buffer[UART_BUFFER_SIZE];


// Transmit data over serial on button press
void uart_transmit(uint8_t);

// Execute function for the module
void serial_task_execute(void);


// Initialising function for the module
void serial_init(void);


#define NUM_BUT_POLLS_1 3
#define NUM_BUT_POLLS_2 3
#define NUM_BUT_POLLS_3 3
#define NUM_BUT_POLLS_4 3

#endif /* INC_SERIAL_H_ */
