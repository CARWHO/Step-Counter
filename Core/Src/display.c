/*
 * display.c
 *
 *  Created on: Mar 10, 2025
 *      Author: Ohu15, Una14
 */

#include "ssd1306.h"
#include "ssd1306_conf.h"
#include "ssd1306_fonts.h"
#include "display.h"
#include "joystick_task.h"
#include "counter_incrementer.h"
#include "test_mode.h"  // For checking test mode status
#include "data_types.h"

#include "state_selector.h"

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>  // For abs()
#include <string.h>

#define BLINKY_FREQ_HZ         4
#define TICK_FREQUENCY_HZ      1000
#define BLINKY_PERIOD_TICKS    (TICK_FREQUENCY_HZ / BLINKY_FREQ_HZ)

// Calibration values for joystick (used in display_joystick_value_setup)
//#define X_CENTER     2284
//#define X_FULL_LEFT  337    // Maps to -100%
//#define X_FULL_RIGHT 4095   // Maps to +100%
//
//#define Y_CENTER     2253
//#define Y_FULL_UP    455   // Maps to +100%
//#define Y_FULL_DOWN  3945   // Maps to -100%
//
//#define VALUE_5 	5
//#define VALUE_10 	10
//#define VALUE_90 	90
//#define VALUE_100 	100

#define LINE_WIDTH 21

static uint32_t displayNextRun = 0;

xy_display get_xy_values(void) {
    return display_joystick_value_setup();
}

void display_init(void) {
    displayNextRun = HAL_GetTick() + BLINKY_PERIOD_TICKS;
    ssd1306_Init();
}


void pad_string(char* dest, size_t dest_size, const char* src, int width) {
    int i = 0;
    while (src[i] != '\0' && i < width && i < dest_size - 1) {
        dest[i] = src[i];
        i++;
    }
    while (i < width && i < dest_size - 1) {
        dest[i] = ' ';
        i++;
    }
    dest[i] = '\0';
}

void joystick_display(xy_display xy_values) {
    char buf[32];
    char padded[33];

    ssd1306_SetCursor(0, 24);
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "X: %d%% %s", xy_values.xPercent, xy_values.x_state);
    pad_string(padded, sizeof(padded), buf, LINE_WIDTH);
    ssd1306_WriteString(padded, Font_7x10, White);

    ssd1306_SetCursor(0, 36);
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "Y: %d%% %s", xy_values.yPercent, xy_values.y_state);
    pad_string(padded, sizeof(padded), buf, LINE_WIDTH);
    ssd1306_WriteString(padded, Font_7x10, White);
}

State state_getter(void) {
    return get_current_state();
}

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

void display_execute(void) {
    uint32_t now = HAL_GetTick();
    if (now >= displayNextRun) {
        state_display(get_xy_values());
        ssd1306_UpdateScreen();

        // If test mode is active, display "TEST" at the bottom of the screen.
        if (test_mode_is_active()) {
            ssd1306_SetCursor(0, 56);
            ssd1306_WriteString("TEST", Font_7x10, White);
        }

        ssd1306_UpdateScreen();
        displayNextRun += BLINKY_PERIOD_TICKS;
    }
}
