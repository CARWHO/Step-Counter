/*
* state_selector.h
*
* Created on: Apr 3, 2025
* Author: una14
*/

#ifndef INC_STATE_SELECTOR_H_
#define INC_STATE_SELECTOR_H_

#include "data_types.h"

// Runs the display and behaviour of current step state
void state_current_step(void);

// Runs the display and behaviour of distance travelled state
State state_dist_travelled(State current_state);

// Runs the display and behaviour of goal progress state
State state_goal_progress(State current_state);

// Runs the display and behaviour of goal edit state
State goal_edit(State current_state);

void state_display(xy_display joystick_attr);
State state_selection(xy_display joystick_attr, State current_state);

// Gives the current state the user is on
State get_current_state(void);


// Sets the state to the new state
void set_current_state(State new_state);


uint32_t sginal_high_time(void);


#endif /* INC_STATE_SELECTOR_H_ */
