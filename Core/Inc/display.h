/*
 * display.h
 *
 *  Created on: Mar 10, 2025
 *      Author: OKH20
 */

#ifndef INC_DISPLAY_H_
#define INC_DISPLAY_H_

#include "data_types.h"


//typedef struct {
//	int16_t xVal;
//	int16_t yVal;
//	int16_t xPercent;
//	int16_t yPercent;
//	const char* x_state;
//	const char* y_state;
//
//} xy_display;

//xy_display display_joystick_value_setup(void);
void joystick_display(xy_display);
void display_init (void);
void display_execute(void);
State state_getter(void);
void pad_string(char* dest, size_t dest_size, const char* src, int width);
xy_display get_xy_values(void);





#endif /* INC_DISPLAY_H_ */
