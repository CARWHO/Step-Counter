/*
 * blinky.c
 *
 *  Created on: Mar 13, 2025
 *      Author: Ohu15, Una14
 */

#include "blinky.h"
#include "stm32c0xx_hal.h"

// For scheduling
#define BLINKY_FREQ_HZ         2
#define TICK_FREQUENCY_HZ      1000
#define BLINKY_PERIOD_TICKS    (TICK_FREQUENCY_HZ / BLINKY_FREQ_HZ)

// Static (module-scope) variable for timing
static uint32_t blinkyNextRun = 0;

void blinky_init(void)
{
    // Initialize or reset any necessary variables
    blinkyNextRun = HAL_GetTick() + BLINKY_PERIOD_TICKS;
}

void blinky_execute(void)
{
    // Check if it is time to run this task
    uint32_t now = HAL_GetTick();
    if (now >= blinkyNextRun)
    {
        HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
        blinkyNextRun += BLINKY_PERIOD_TICKS;
    }

}

