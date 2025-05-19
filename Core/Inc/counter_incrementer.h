/*
 * counter_incrementer.h
 *
 *  Created on: Apr 3, 2025
 *      Author: OKH20
 */

#ifndef INC_COUNTER_INCREMENTER_H_
#define INC_COUNTER_INCREMENTER_H_

#include <stdint.h>


// Initializes the step counter, distance, and goal to default values.
void counter_incrementer_init(void);

// Getters
uint32_t counter_incrementer_get_steps(void);
uint32_t* counter_incrementer_get_distance(void);
uint32_t counter_incrementer_get_goal(void);
uint32_t step_to_km_float(void);


// Setters
void counter_incrementer_set_goal(uint32_t newGoal);
uint16_t set_goal(void);



void counter_incrementer_update_goal(uint16_t new_goal);



// Get goal percentage
uint32_t counter_incrementer_get_percentage_goal(void);

// Increment/Decrement helpers
void counter_incrementer_increment_steps(uint32_t stepDelta);
void counter_incrementer_decrement_steps(uint32_t stepDelta);

#endif /* INC_COUNTER_INCREMENTER_H_ */
