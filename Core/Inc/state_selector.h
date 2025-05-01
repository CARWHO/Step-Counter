/*
* state_selector.h
*
* Created on: Apr 3, 2025
* Author: una14
*/

#ifndef INC_STATE_SELECTOR_H_
#define INC_STATE_SELECTOR_H_

#include "data_types.h"

void state_current_step(void);
State state_dist_travelled(State current_state);
State state_goal_progress(State current_state);
State goal_edit(State current_state);

State state_selection(xy_display joystick_attr, State current_state);
State get_current_state(void);
void set_current_state(State new_state);

uint32_t sginal_high_time(void);

//static uint32_t current_steps = 0;
//static uint32_t current_distance = 0;
//static uint32_t step_goal = 200;

#endif /* INC_STATE_SELECTOR_H_ */
