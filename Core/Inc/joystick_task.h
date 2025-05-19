#ifndef JOYSTICK_TASK_H
#define JOYSTICK_TASK_H
#include <stdint.h>
#include <stdbool.h>
#include "data_types.h"


// Initialising function for the module
void joystick_task_init(void);

// Execute function for the module
void joystick_task_execute(void);


// Getters for x and y joystick value
uint16_t joystick_X_val(void);
uint16_t joystick_Y_val(void);


// Detecting joystick press
bool joystick_click_pressed(void);

/*
 *  Returns struct containing coord's
 *  attributes
 *  adc value,
 *  percentage of its direction 100% or -100%
 *  position in string -> rest, UP, DOWN, right, left
 */
xy_display get_xy_values(void);


#endif
