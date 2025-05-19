/*
 * test_mode.c
 *
 *  Created on: Apr 3, 2025
 *      Author: ohu15, una14
 */

#include "test_mode.h"
#include "counter_incrementer.h"
#include "joystick_task.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "display.h"
#include "stm32c0xx_hal.h"

// These constants mirror the joystick calibration from display.c.
#define Y_CENTER     2253
#define Y_FULL_UP    455    // Maps to +100%
#define Y_FULL_DOWN  3945   // Maps to -100%
#define DEADZONE_THRESHOLD 50


// At maximum displacement, we want about 3 steps per update.
// With an update period of 50ms (20Hz), that yields roughly 60 steps per second.
#define MAX_STEPS_PER_TICK 3

// Update period for test mode (in milliseconds)
#define TEST_MODE_PERIOD_MS 	50
#define MULTIPLIER				18
#define VALUE_100				100


static uint32_t 	test_mode_next_run 	= 0;

// Test mode status
static bool 		test_mode_active 	= false;

void test_mode_init(void) {
    test_mode_active = false;
    test_mode_next_run = 0;
}

/*
 * Toggling the test mode when user double pressed SW2
 */
void test_mode_toggle(void) {

	test_mode_active = !test_mode_active;

    // Reset the timer when toggling mode to avoid an immediate update.
    test_mode_next_run = HAL_GetTick() + TEST_MODE_PERIOD_MS;
}

/*
 * Function return the status of the test mode
 */
bool test_mode_is_active(void) {
    return test_mode_active;
}

/*
 *  Increment steps function to modularise button_update_step()
 *  adds the delta value to step count towards goal
 */
void increment_step(uint32_t delta) {

	        uint32_t currentSteps = counter_incrementer_get_steps();
	        uint32_t goal = counter_incrementer_get_goal();

	        // Only increment if goal not reached.
	        if (currentSteps < goal) {

	        	uint32_t newSteps = currentSteps + delta;

	        	if (newSteps > goal) {

	        		newSteps = goal;
	            }

	            counter_incrementer_increment_steps(newSteps - currentSteps);
	        }
}

// Decrement steps: if x-axis exceeds the y-axis.
void decrement_step(uint32_t delta) {
        uint32_t currentSteps = counter_incrementer_get_steps();

        // Only decrement if there are steps to subtract.
        if (currentSteps > 0) {
            uint32_t newSteps = currentSteps - delta;

            // Prevent underflow.
            if (newSteps > currentSteps) {
                newSteps = 0;
            }

            counter_incrementer_decrement_steps(currentSteps - newSteps);
    }
}

/*
 * When in test mode.
 * Adding proportional increment/ decrement to the position of the joystick
 *
 */
void button_update_step(xy_display data) {

    /*
     * Appling deadzone thresholds to x and y axes.
     */
    if (abs(data.xPercent) < DEADZONE_THRESHOLD) {
        data.xPercent = 0;
    }

    if (abs(data.yPercent) < DEADZONE_THRESHOLD) {
        data.yPercent = 0;
    }



    // Compute a delta proportional to the displacement:
    int delta = ((abs(data.xPercent) * MAX_STEPS_PER_TICK) / VALUE_100) * MULTIPLIER;





    /*
     * Increase/ decrease number of steps using
     * the position of the joystick from x and y percentages.
     *
     * x and y percentages were used because it
     * will make the code simple and readable as
     * percentage value is taken after processing
     * joystick analogue value
     */
    if (data.xPercent < data.yPercent) {

    	increment_step(delta);

    } else if (data.xPercent > data.yPercent){

    	decrement_step(delta);

    }

}

// Task scheduling for test mode
void test_mode_execute(void) {


    uint32_t now = HAL_GetTick();

    // checking timing requirement met and test mode is active

    if (now >= test_mode_next_run && test_mode_active){


    	button_update_step(get_xy_values());

		test_mode_next_run = now + TEST_MODE_PERIOD_MS;
    }
}


