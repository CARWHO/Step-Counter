/*
 * joystick_task.c
 *
 *  Created on: Mar 11, 2025
 *      Author: Ohu15, Una14
 */

#include "joystick_task.h"
#include "stm32c0xx_hal.h"
#include "adc.h"
#include "data_types.h"
#include <stdint.h>
#include <stdbool.h>

#define JOYSTICK_POLL_FREQ     20
#define TICK_FREQUENCY_HZ      1000
#define JOYSTICK_PERIOD_TICKS  (TICK_FREQUENCY_HZ / JOYSTICK_POLL_FREQ)

static uint32_t joystickNextRun = 0;
uint16_t raw_adc[3];

void joystick_task_init(void)
{
    joystickNextRun = HAL_GetTick() + JOYSTICK_PERIOD_TICKS;
}

void joystick_task_execute(void)
{
    uint32_t now = HAL_GetTick();
    if (now >= joystickNextRun)
    {
        // Trigger the ADC for joystick
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)raw_adc, 3);
        joystickNextRun += JOYSTICK_PERIOD_TICKS;
    }
}

// Getter functions
uint16_t joystick_Y_val(void) {
    return raw_adc[2];  // Assuming channel 0 is Y
}

uint16_t joystick_X_val(void) {
    return raw_adc[1];  // Assuming channel 1 is X
}

bool joystick_click_pressed(void) {
    return (HAL_GPIO_ReadPin(JOYSTICK_CLICK_GPIO_Port, JOYSTICK_CLICK_Pin) == GPIO_PIN_RESET);
}


xy_display display_joystick_value_setup(void) {
    uint16_t xVal = joystick_X_val();
    uint16_t yVal = joystick_Y_val();
    int16_t xPercent, yPercent;

    if (xVal >= X_CENTER) {
        xPercent = - ((xVal - X_CENTER) * VALUE_100) / (X_FULL_LEFT - X_CENTER);
    } else {
        xPercent = ((X_CENTER - xVal) * VALUE_100) / (X_CENTER - X_FULL_RIGHT);
    }

    if (yVal >= Y_CENTER) {
        yPercent = - ((yVal - Y_CENTER) * VALUE_100) / (Y_FULL_DOWN - Y_CENTER);
    } else {
        yPercent = ((Y_CENTER - yVal) * VALUE_100) / (Y_CENTER - Y_FULL_UP);
    }

    if (xPercent > 100) xPercent = 100;
    if (xPercent < -100) xPercent = -100;
    if (yPercent > 100) yPercent = 100;
    if (yPercent < -100) yPercent = -100;

    if (xPercent > VALUE_90) {
        xPercent = 100;
    } else if (xPercent < - VALUE_90) {
        xPercent = -100;
    }
    if (yPercent > VALUE_90) {
        yPercent = 100;
    } else if (yPercent < - VALUE_90) {
        yPercent = -100;
    }

    if (abs(xPercent) < VALUE_5) {
        xPercent = 0;
    }
    if (abs(yPercent) < VALUE_5) {
        yPercent = 0;
    }

    const char* x_state;
    if (xPercent > VALUE_10) {
        x_state = "right";
    } else if (xPercent < -VALUE_10) {
        x_state = "left";
    } else {
        x_state = "rest";
    }

    const char* y_state;
    if (yPercent > VALUE_10) {
        y_state = "UP";
    } else if (yPercent < -VALUE_10) {
        y_state = "DOWN";
    } else {
        y_state = "rest";
    }

    xy_display xy_values = {
        .xVal = xVal,
        .yVal = yVal,
        .xPercent = xPercent,
        .yPercent = yPercent,
        .x_state = x_state,
        .y_state = y_state
    };
    return xy_values;
}
