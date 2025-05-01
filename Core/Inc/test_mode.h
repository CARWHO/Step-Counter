/*
 * test_mode.h
 *
 *  Created on: Apr 3, 2025
 *      Author: OKH20
 */

#ifndef INC_TEST_MODE_H_
#define INC_TEST_MODE_H_

#include <stdbool.h>

void test_mode_init(void);
void test_mode_toggle(void);
void test_mode_execute(void);
bool test_mode_is_active(void);

#endif /* INC_TEST_MODE_H_ */
