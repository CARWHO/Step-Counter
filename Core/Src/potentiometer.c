/*
 * joystick_task.c
 *
 *  Created on: Mar 11, 2025
 *      Author: Ohu15, Una14
 */

#include "potentiometer.h"
#include "stm32c0xx_hal.h"
#include "adc.h"
#include "data_types.h"

#include <stdint.h>

#define POTEN_POLL_FREQ     20
#define TICK_FREQUENCY_HZ      1000
#define POTENTIOMETER_PERIOD_TICKS  (TICK_FREQUENCY_HZ / POTEN_POLL_FREQ)

static uint32_t potentiometerNextRun = 0;

void potentiometer_task_init(void)
{
	potentiometerNextRun = HAL_GetTick() + POTENTIOMETER_PERIOD_TICKS;
}

void potentiometer_task_execute(void)
{
    uint32_t now = HAL_GetTick();
    if (now >= potentiometerNextRun)
    {
        potentiometerNextRun += POTENTIOMETER_PERIOD_TICKS;
    }
}

// Getter functions for potentiometer
uint16_t potentiometer_val(void) {
    return raw_adc[0];
}
