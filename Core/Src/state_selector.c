/*
 * Selecting states from user Joystick input
 */
// std libs
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// self header
#include "state_selector.h"
#include "main.h"

// Ext modules
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

void set_current_state(State new_state) {
    internal_current_state = new_state;
}

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

void state_current_step(void) {
    char stepBuf[32];
    char paddedStep[50];
//    char goalBuf[50];
	ssd1306_Fill(Black);
    ssd1306_SetCursor(5, 0);
    ssd1306_WriteString("Current step", Font_7x10, White);
    snprintf(stepBuf, sizeof(stepBuf), "Steps: %lu", counter_incrementer_get_steps());
    pad_string(paddedStep, sizeof(paddedStep), stepBuf, 12);
    ssd1306_SetCursor(5, 14);
    ssd1306_WriteString(paddedStep, Font_7x10, White);

}


State state_dist_travelled(State current_state) {
    char buf[32];
    char padded[33];
    static bool toggle_initiated = false;
    static uint8_t distance_index = 0;

    ssd1306_Fill(Black);
    ssd1306_SetCursor(5, 0);
    ssd1306_WriteString("Dist Travell", Font_7x10, White);

    // Get the current joystick state (assumes get_xy_values() is available)
    xy_display data = get_xy_values();
    uint32_t* distances = counter_incrementer_get_distance();

    // Toggle the index on a rising edge when the joystick is pushed right.
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
        // index 0 is kilometers.
        snprintf(buf, sizeof(buf), "%lu km", distances[0]);
    } else {
        // index 1 is yards.
        snprintf(buf, sizeof(buf), "%lu yd", distances[1]);
    }
    pad_string(padded, sizeof(padded), buf, LINE_WIDTH);

    ssd1306_SetCursor(5, 10);
    ssd1306_WriteString(padded, Font_7x10, White);

    return current_state;
}

State goal_edit(State current_state) {
    static bool first_entry = true;
    static uint16_t previous_goal = 0;
    static bool waiting_for_button_release = true;
    static bool new_press_detected = false;
    static uint32_t press_start_time = 0;
    uint32_t current_time = HAL_GetTick();

    // Disabling SW1 and SW2
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1);
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_11);

    // Eentering the edit state, save the previous goal
    if (first_entry) {
        previous_goal = counter_incrementer_get_goal();
        first_entry = false;
        waiting_for_button_release = true;  // Wait for release of the button that got us here
        new_press_detected = false;
    }

    // Clear the screen and display the "Goal Edit" header.
    ssd1306_Fill(Black);
    ssd1306_SetCursor(5, 0);
    ssd1306_WriteString("Goal Edit", Font_7x10, White);

    // Prepare buffers to display current steps and goal.
    char stepBuf[40], paddedStep[50], goalBuf[50];

    // Display current potentiometer value (which will become the new goal)
    uint16_t potential_new_goal = set_goal(); // This reads from raw_adc[0]

    // Retrieve and format the current step count.
    snprintf(stepBuf, sizeof(stepBuf), "Steps: %lu", counter_incrementer_get_steps());
    pad_string(paddedStep, sizeof(paddedStep), stepBuf, 15);

    // Format the potential new goal
    snprintf(goalBuf, sizeof(goalBuf), "New Goal: %u", potential_new_goal);

        // Display the formatted step count and goal.
        ssd1306_SetCursor(5, 14);
        ssd1306_WriteString(paddedStep, Font_7x10, White);
        ssd1306_SetCursor(5, 25);
        ssd1306_WriteString(goalBuf, Font_7x10, White);
        ssd1306_SetCursor(5, 36);

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

State state_goal_progress(State current_state) {
    char stepBuf[32];
    char paddedStep[50];
    char goalBuf[50];

    static uint32_t press_start_time = 0;
    static bool long_press_triggered = false;  // Flag to indicate that a long press event has been handled
    uint32_t current_time = HAL_GetTick();

    ssd1306_Fill(Black);
    ssd1306_SetCursor(5, 0);
    ssd1306_WriteString("Goal Progress", Font_7x10, White);

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // GPIOC clock is enabled
    __HAL_RCC_GPIOC_CLK_ENABLE();

    // Configuring PC1 (SW2) as a GPIO input (pull down)
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

    // Format and display the current step count.
    snprintf(stepBuf, sizeof(stepBuf), "Steps: %lu", counter_incrementer_get_steps());
    pad_string(paddedStep, sizeof(paddedStep), stepBuf, 12);

    // Format and display the current goal.
    snprintf(goalBuf, sizeof(goalBuf), "Goal: %lu", counter_incrementer_get_goal());
    ssd1306_SetCursor(5, 14);
    ssd1306_WriteString(paddedStep, Font_7x10, White);
    ssd1306_SetCursor(5, 25);
    ssd1306_WriteString(goalBuf, Font_7x10, White);


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
