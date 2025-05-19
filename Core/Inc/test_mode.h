/*
 * test_mode.h
 *
 *  Created on: Apr 3, 2025
 *      Author: OKH20
 */

#ifndef INC_TEST_MODE_H_
#define INC_TEST_MODE_H_

#include <stdint.h>
#include <stdio.h>

#include <stdbool.h>


// Initialising function for the module
void test_mode_init(void);
void test_mode_toggle(void);

// Execute function for the module
void test_mode_execute(void);


void increment_step(uint32_t delta);
void incrementstep(uint32_t delta);

// Checks if test mode is active
bool test_mode_is_active(void);

#endif /* INC_TEST_MODE_H_ */
