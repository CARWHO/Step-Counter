/*
 * state_selector.c
 *
 *  Created on: Mar 10, 2025
 *      Author: Ohu15, Una14
 */

#define PERIOD_ONE_SECOND_IN_MS 1000 // Define 1 second in milliseconds

// std libs
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdarg.h>


#include "state_selector.h"
#include "joystick_task.h"
#include "main.h"

#include "ssd1306.h"
#include "ssd1306_conf.h"
#include "ssd1306_fonts.h"

#include "counter_incrementer.h"
#include "display.h"

#define DEADZONE_THRESHOLD 80  // Adjust this value to make the deadzone larger
#define LINE_WIDTH 21


// Maintain state within this file
static State internal_current_state = STATE_STATE_CURRENT_STEP;

// Getter and setter for the current state
State get_current_state(void) {
    return internal_current_state;
}

// Setter function for display state
void set_current_state(State new_state) {
    internal_current_state = new_state;
}

// Changing state depending on joystick position
void state_display(xy_display joystick_attr) {
    State current_state = get_current_state();
    current_state = state_selection(joystick_attr, current_state);

    switch(current_state) {
        case STATE_STATE_CURRENT_STEP:
            state_current_step();
            break;

        case STATE_DIST_TRAVELLED:
            current_state = state_dist_travelled(current_state);
            break;

        case STATE_GOAL_PROGRESS:
            current_state = state_goal_progress(current_state);
            break;

        case STATE_GOAL_EDIT:
            current_state = goal_edit(current_state);
            break;
    }

    // Update the central state storage
    set_current_state(current_state);
}

// Changes to new state depending on current state and joystick position
State state_selection(xy_display data, State current_state) {
    // Apply deadzone threshold to the Y axis.
    if (abs(data.yPercent) < DEADZONE_THRESHOLD) {
        data.yPercent = 0;
    }

    // Only update state if the Y input exceeds the deadzone.
    // Here, a positive Y value cycles forward and a negative Y value cycles backward.
    if (data.yPercent > DEADZONE_THRESHOLD) {
        // Joystick moved upward (assuming positive Y indicates upward)

    	switch (current_state) {

    	case STATE_STATE_CURRENT_STEP:

    			current_state = STATE_GOAL_PROGRESS;
                break;

            case STATE_GOAL_PROGRESS:

            	current_state = STATE_DIST_TRAVELLED;
                break;

            case STATE_DIST_TRAVELLED:

            	current_state = STATE_STATE_CURRENT_STEP;
                break;

            case STATE_GOAL_EDIT:
                // No change when in edit mode
                break;
        }
    } else if (data.yPercent < -DEADZONE_THRESHOLD) {

    	// Joystick moved downward (assuming negative Y indicates downward)
    	switch (current_state) {

    	case STATE_STATE_CURRENT_STEP:

    		current_state = STATE_DIST_TRAVELLED;
                break;

            case STATE_GOAL_PROGRESS:

            	current_state = STATE_STATE_CURRENT_STEP;
                break;

            case STATE_DIST_TRAVELLED:

            	current_state = STATE_GOAL_PROGRESS;
                break;

            case STATE_GOAL_EDIT:
                // No change when in edit mode
                break;
        }
    }

    return current_state;
}



/*
 * Display and logic when in Current Step state
 */
void state_current_step(void) {


	display_title("Current step");

	static bool toggle_initiated = false;
    static uint8_t distance_index = 0;

    // Get the current joystick state (assumes get_xy_values() is available)
    xy_display data = get_xy_values();


    /*
     *  Toggle the index on a rising edge when the joystick is
     *	pushed right. To change display for steps to percentage
     */
	if (-data.xPercent > DEADZONE_THRESHOLD) {

    	if (!toggle_initiated) {

    		// Toggle index: if currently 0 then switch to 1, otherwise switch to 0.
                distance_index = (distance_index == 1) ? 0 : 1;
                toggle_initiated = true;
            }
        } else {
            // Reset the toggle flag when the joystick is no longer pushed right.
            toggle_initiated = false;
        }

    // Format the selected distance value with the appropriate unit.
    if (distance_index == 0) {
    		format_display(5, 14, "Steps: %lu", counter_incrementer_get_steps());
    } else {

    		format_display(5, 14, "Goal %%: %lu%%", counter_incrementer_get_percentage_goal());
    }
}


/*
 * Display and logic when in Distance travelled state
 */

State state_dist_travelled(State current_state) {
    static bool toggle_initiated = false;
    static uint8_t distance_index = 0;

    display_title("Dist Travelled");

    // Get the current joystick state (assumes get_xy_values() is available)
    xy_display data = get_xy_values();
    uint32_t* distances = counter_incrementer_get_distance();


    /*
     *  Toggle the index on a rising edge when the joystick is pushed right.
     *  To change from km to yds when joystick is move up
     */
    if (-data.xPercent > DEADZONE_THRESHOLD) {
            if (!toggle_initiated) {
                // Toggle index: if currently 0 then switch to 1, otherwise switch to 0.
                distance_index = (distance_index == 1) ? 0 : 1;
                toggle_initiated = true;
            }
        } else {
            // Reset the toggle flag when the joystick is no longer pushed right.
            toggle_initiated = false;
        }


    if (distance_index == 0) {

    	// Writing on screen the distance in km to 1dp
    	   char stepBuf[32];
    	    char paddedStep[50];

    	    snprintf(stepBuf, sizeof(stepBuf), "%lu.%lu km", distances[0], distances[2]);
    	    pad_string(paddedStep, sizeof(paddedStep), stepBuf, 12);

    	    ssd1306_SetCursor(5, 14);
    	    ssd1306_WriteString(paddedStep, Font_7x10, White);


    } else {
        // index 1 is yards.
    	// displaying distance in yards
    		format_display(5, 10, "%lu yd", distances[1]);
    }

    return current_state;
}


/*
 * Display and logic when in goal edit state
 */
State goal_edit(State current_state) {

	// flags for detection
    static bool first_entry = true;
    static uint16_t previous_goal = 0;
    static bool waiting_for_button_release = true;
    static bool new_press_detected = false;
    static uint32_t press_start_time = 0;
    uint32_t current_time = HAL_GetTick();


    // Disabling SW1 and SW2 to disable button's functionality in goal edit
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11);

    // Entering the edit state, save the previous goal
    if (first_entry) {

    	previous_goal = counter_incrementer_get_goal();
        first_entry = false;
        waiting_for_button_release = true;  // Wait for release of the button that got us here
        new_press_detected = false;
    }

    // Clear the screen and display the "Goal Edit" header.
    display_title("Goal Edit");

    // Prepare buffers to display current steps and goal.

    // Display current potentiometer value (which will become the new goal)
    uint16_t potential_new_goal = set_goal(); // This reads from raw_adc[0]

    // Displaying new goal and steps
    format_display(5, 14, "Steps: %lu", counter_incrementer_get_steps());
    format_display(5, 25, "New Goal: %u", potential_new_goal);



    // Read the joystick button state.
    bool button_pressed = (HAL_GPIO_ReadPin(JOYSTICK_CLICK_GPIO_Port, JOYSTICK_CLICK_Pin) == GPIO_PIN_SET);

    // Handle the initial button release after entering this state
    if (waiting_for_button_release) {

    	if (!button_pressed) {

    		waiting_for_button_release = false;
        }
        return current_state;  // Stay in this state
    }

    // Handle new button presses for save/cancel
    if (button_pressed) {

    	if (!new_press_detected) {

    		// Button was just pressed, record the start time
            press_start_time = current_time;
            new_press_detected = true;
        }


    }
    // Button released: determine what action to take based on press duration
    else if (new_press_detected) {

    	uint32_t press_duration = current_time - press_start_time;

        if (press_duration >= PERIOD_ONE_SECOND_IN_MS) {

        	// Long press: save the new goal
            counter_incrementer_update_goal(potential_new_goal);
        } else {

        	// Short press - revert to previous goal
            counter_incrementer_update_goal(previous_goal);
        }

        // Reset tracking variables
        new_press_detected = false;
        first_entry = true; // Reset for next time we enter edit mode

        // Return to goal progress state
        current_state = STATE_GOAL_PROGRESS;
    }

    return current_state;
}


/*
 * Display and logic when in Goal progress state
 */
State state_goal_progress(State current_state) {

    static uint32_t press_start_time = 0;
    static bool long_press_triggered = false;  // Flag to indicate that a long press event has been handled
    uint32_t current_time = HAL_GetTick();

    // Title
    display_title("Goal Progress");

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // GPIOC clock is enabled
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configuring PC1 (SW2) as a GPIO input (pull down)
    /*
     *  Re-enabling the pins disabled in goal edit mode
     *  When exiting goal edit mode
     *  Since to go in goal edit mode you have to be in goal
     *  progress
     */
    GPIO_InitStruct.Pin   = GPIO_PIN_1;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Configure PC11 (SW1) as a GPIO input (pull down)
    GPIO_InitStruct.Pin   = GPIO_PIN_11;
    GPIO_InitStruct.Mode  = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull  = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    // Read the joystick button state.
    bool button_pressed = (HAL_GPIO_ReadPin(JOYSTICK_CLICK_GPIO_Port, JOYSTICK_CLICK_Pin) == GPIO_PIN_SET);


    format_display(5, 14, "Steps: %lu", counter_incrementer_get_steps());
    format_display(5, 25, "Goal: %lu", counter_incrementer_get_goal());

    // If the button is pressed, handle timing logic.
    if (button_pressed) {

    	// Record the start time on the initial press.
        if (press_start_time == 0) {

        	press_start_time = current_time;
        }
        // If we've held the button for more than 1000 ms and haven't yet triggered the action.
        if ((current_time - press_start_time > PERIOD_ONE_SECOND_IN_MS) && !long_press_triggered) {

        	current_state = STATE_GOAL_EDIT;
            long_press_triggered = true;
        }
    }
    // If the button is released, reset the timer and flag.
    else {

        press_start_time = 0;
        long_press_triggered = false;
    }

    return current_state;
}
