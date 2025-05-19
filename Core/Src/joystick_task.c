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
#include <stdlib.h>
#include <stdbool.h>


#define JOYSTICK_POLL_FREQ     20
#define TICK_FREQUENCY_HZ      1000
#define JOYSTICK_PERIOD_TICKS  (TICK_FREQUENCY_HZ / JOYSTICK_POLL_FREQ)


#define RAW_ADC_LENGTH 3

// Threshold value MACROs
#define POSITION_REST						"rest"
#define REST_PERCENTAGE_VALUE 				((int8_t) 0)
#define MAX_POSITIVE_PERCENTAGE_VALUE 		((int8_t) 100)
#define MAX_NEGATIVE_PERCENTAGE_VALUE 		((int8_t) -100)

#define CLAMP_REST_POSITIVE_PERCENTAGE		((int8_t) 10)
#define CLAMP_REST_NEGATIVE_PERCENTAGE		((int8_t) -10)
#define CLAMP_POSITIVE_PERCENTAGE 			((int8_t) 80)
#define CLAMP_NEGATIVE_PERCENTAGE 			((int8_t) -80)

#define POSITIVE_DIRECTION_THRES			((int8_t)10)
#define NEGATIVE_DIRECTION_THRES			((int8_t)-10)
#define PERCENT								((int8_t)100)


static uint32_t joystickNextRun = 0;
uint16_t raw_adc[3];

void joystick_task_init(void)
{
    joystickNextRun = HAL_GetTick() + JOYSTICK_PERIOD_TICKS;
}


/*
 * Execute function for the joystick task
 */
void joystick_task_execute(void)
{
    uint32_t now = HAL_GetTick();
    if (now >= joystickNextRun)
    {
        // Trigger the ADC for joystick
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)raw_adc, RAW_ADC_LENGTH);
        joystickNextRun += JOYSTICK_PERIOD_TICKS;
    }
}

// Getter functions for x and y joystick values
uint16_t joystick_Y_val(void) {
    return raw_adc[2];  // Assuming channel 0 is Y
}

uint16_t joystick_X_val(void) {
    return raw_adc[1];  // Assuming channel 1 is X
}


bool joystick_click_pressed(void) {
    return (HAL_GPIO_ReadPin(JOYSTICK_CLICK_GPIO_Port, JOYSTICK_CLICK_Pin) == GPIO_PIN_RESET);
}



int16_t convert_adc_joystick_to_percentage(uint16_t coord_number, uint16_t center_number, uint16_t full_left, uint16_t full_right){

	int16_t percentage;
	if (coord_number >= center_number) {
		percentage = - ((coord_number - center_number) * PERCENT) / (full_left - center_number);
    } else {
    	percentage = ((center_number - coord_number) * PERCENT) / (center_number - full_right);
    }

	return percentage;

}



/*
 * Clamps value of the given number
 * Used for percentage of x and y axis
 */
int8_t clamp_value_number(int8_t number) {

	int8_t percentage = number;

	if (number >= CLAMP_POSITIVE_PERCENTAGE) {

		percentage = MAX_POSITIVE_PERCENTAGE_VALUE;

	} else if (number <= CLAMP_NEGATIVE_PERCENTAGE){

		percentage = MAX_NEGATIVE_PERCENTAGE_VALUE;

		/*
		 * Adding Deadzone
		 */
	} else if ((number >= CLAMP_REST_NEGATIVE_PERCENTAGE) && (number <= CLAMP_REST_POSITIVE_PERCENTAGE)) {

		percentage = REST_PERCENTAGE_VALUE;
	}

	return percentage;
}



char* set_direction(int16_t percentage, char* primary_direction, char* secondary_direction) {

	char* direction;

	if (percentage > POSITIVE_DIRECTION_THRES) {

		direction = primary_direction;

	} else if ( percentage < (NEGATIVE_DIRECTION_THRES)) {

		direction = secondary_direction;

	} else {
		direction =  POSITION_REST;
	}

	return direction;

}

xy_display get_xy_values(void) {

    uint16_t xVal 	= joystick_X_val();
    uint16_t yVal 	= joystick_Y_val();

    // Getting percentage for x and y joystick
    int8_t xPercent = convert_adc_joystick_to_percentage(xVal, X_CENTER, X_FULL_LEFT, X_FULL_RIGHT);
    xPercent 		= clamp_value_number(xPercent);

    int8_t yPercent = convert_adc_joystick_to_percentage(yVal, Y_CENTER, Y_FULL_DOWN, Y_FULL_UP);
    yPercent 		= clamp_value_number(yPercent);

    // Getting directions for x and y joystick
    char* x_state 	= set_direction(xPercent, "right", "left");
    char* y_state 	= set_direction(yPercent, "UP", "DOWN");

    // Creating struct of the collected information
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
