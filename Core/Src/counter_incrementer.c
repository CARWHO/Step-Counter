/*
 * counter_incrementer.c
 *
 *  Created on: Apr 3, 2025
 *      Author: ohu15
 */

#include "counter_incrementer.h"
#include "data_types.h"

// Internal (static) variables to track the current step count, distance, and goal
static uint32_t current_steps = 0;
static uint32_t current_distance = 0;
static uint32_t step_goal = 1000;   // Default goal
// Note: 'units' is declared locally in the distance getter below

void counter_incrementer_init(void)
{
    current_steps = 0;
    current_distance = 0;
    step_goal = 1000;   // Default goal
}

uint32_t counter_incrementer_get_steps(void)
{
    return current_steps;
}

uint32_t* counter_incrementer_get_distance(void)
{
    // We return two values: index 0 for kilometers, index 1 for yards.
    static uint32_t units[2];
    uint32_t steps = counter_incrementer_get_steps();

    // km ≈ (steps * 9) / 10000.
//    units[0] = (steps * 9) / 10000;
    units[0] = STEPS_TO_KM(steps);
    // yards: 1 m ≈ 1.09394 yards, approximate by multiplying by 1094.
//    units[1] = (steps * 9 * 1094) / 10000;
    units[1] = STEPS_TO_YARDS(steps);

    return units;
}

uint32_t counter_incrementer_get_goal(void)
{
    return step_goal;
}

void counter_incrementer_set_goal(uint32_t newGoal)
{
    step_goal = newGoal;
}

void counter_incrementer_increment_steps(uint32_t stepDelta)
{
    current_steps += stepDelta;
}

void counter_incrementer_decrement_steps(uint32_t stepDelta)
{
    if (current_steps >= stepDelta)
        current_steps -= stepDelta;
    else
        current_steps = 0;
}


// Use the potentiometer ADC reading (raw_adc[0]) to compute a new goal.
// Assumes raw_adc[0] ranges from 0 to 4095.
// Scales this to a goal between 0 and 15000.

uint16_t set_goal(void) {
	uint16_t goal_new = CONV_ADC_TO_GOAL(raw_adc[0]);
	return INTEGER_ARITHMETHIC_250(goal_new);
}


// Update the goal with the new value.
// Here we update the same 'step_goal' variable used by counter_incrementer_get_goal.

void counter_incrementer_update_goal(uint16_t new_goal) {
    step_goal = new_goal;
}
