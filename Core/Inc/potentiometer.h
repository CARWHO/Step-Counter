/*
 * potentiometer.h
 *
 *  Created on: Apr 6, 2025
 *      Author: Ohu15, Una14
 */
#include <stdint.h>

#ifndef INC_POTENTIOMETER_H_
#define INC_POTENTIOMETER_H_

void potentiometer_task_init(void);
void potentiometer_task_execute(void);
uint16_t potentiometer_val(void);

#endif /* INC_POTENTIOMETER_H_ */
