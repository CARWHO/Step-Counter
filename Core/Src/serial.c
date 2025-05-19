/*
 * serial.c
 *
 *  Created on: May 6, 2025
 *      Author: una14
 */

//#include "button_task.h"
#include "buttons.h"
#include "peak_detect.h"
#include "button_task.h"
#include "rgb.h"
#include "stm32c0xx_hal.h"
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h> // Added for PRId64
#include "serial.h"
#include "joystick_task.h"
#include "usart.h"
#include "display.h"
#include "counter_incrementer.h"
#include "potentiometer.h"
#include "test_mode.h"
#include "imu_sensor.h"
#include "state_selector.h" // Added for get_current_state()

#include <string.h>

// For scheduling
#define BUTTON_POLL_FREQ       500
#define TICK_FREQUENCY_HZ      1000
#define BUTTON_PERIOD_TICKS    (TICK_FREQUENCY_HZ / BUTTON_POLL_FREQ)
#define INC_DUTYCYCLE          10
#define TIME_OUT_VALUE         50

char uart_buffer[UART_BUFFER_SIZE];

static uint32_t buttonNextRun = 0;

void serial_init(void) {
    buttonNextRun = HAL_GetTick() + BUTTON_PERIOD_TICKS;
}

void uart_transmit(uint8_t flag) {
	if (flag == 1 ){
        int64_t mag2_val = peak_detect_get_mag2();
        uint32_t mag2_upper = (uint32_t)(mag2_val >> 32);
        uint32_t mag2_lower = (uint32_t)(mag2_val & 0xFFFFFFFFUL);

        snprintf(uart_buffer, sizeof(uart_buffer),
        		   "\n%lu\n", // Raw mag2 value (split into upper/lower 32-bit parts)
				   mag2_lower
				 );
        /*
         * .xVal = xVal,
        .yVal = yVal,
        .xPercent = xPercent,
        .yPercent = yPercent,
        .x_state = x_state,
        .y_state = y_state
         * */
        HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer,
                          strlen(uart_buffer), TIME_OUT_VALUE);
    }
}

void serial_task_execute(void)
{
    uint32_t now = HAL_GetTick();
    if (now >= buttonNextRun)
    {
    	uart_transmit(UART_button_state_get());


        buttonNextRun += BUTTON_PERIOD_TICKS;
    }
}
