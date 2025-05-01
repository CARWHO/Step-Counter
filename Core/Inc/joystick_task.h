#ifndef JOYSTICK_TASK_H
#define JOYSTICK_TASK_H
#include <stdint.h>
#include <stdbool.h>
#include "data_types.h"



void joystick_task_init(void);
void joystick_task_execute(void);
uint16_t joystick_X_val(void);
uint16_t joystick_Y_val(void);
bool joystick_click_pressed(void);
xy_display display_joystick_value_setup(void);

#endif
