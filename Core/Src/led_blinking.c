/*
 * led_task.c
 */

#include "led_blinking.h"
#include "pwm.h"
#include "counter_incrementer.h"
#include "stm32c0xx_hal.h"
#include "rgb.h"
#include "stdbool.h"


#define TEST_MODE_POLL_FREQ       	20
#define TICK_FREQUENCY_HZ      		1000
#define TEST_MODE_PERIOD_TICKS    	(TICK_FREQUENCY_HZ / TEST_MODE_POLL_FREQ)





// Setting the duty cycle of DS3 when initialising led_blinking.c
#define DS3_INITIAL_DUTY 			0

/*
 * LED goal completion threshold
 * Numbering of the is in order of
 * which LED turns on after DS3
 */
#define LED_2_THRESHOLD 			50
#define LED_3_THRESHOLD 			75
#define LED_4_THRESHOLD 			100


static uint32_t test_mode_next_run = 0;




void led_init(void)
{
    HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_3);
    pwm_setDutyCycle(&htim2, TIM_CHANNEL_3, DS3_INITIAL_DUTY);
}

void led_execute(void)
{
	/*
	 * Setting the scheduling of the task
	 *
	 */
	uint32_t now = HAL_GetTick();
	    if (now >= test_mode_next_run)
	    {

		uint32_t percent = counter_incrementer_get_percentage_goal();

		/*
		 * Setting duty cycle for DS3 brightness related to the
		 * goal completion of 25%
		 */
		pwm_setDutyCycle(&htim2, TIM_CHANNEL_3, percent);

		// Turning all LEDs off
		for (int i = 0; i < RGB_NUM_LEDS; i++) {
			rgb_led_off((rgb_led_t)i);
		}

		/*
		 * Turning on LEDs when their corresponding goal
		 * completion threshold is reached
		 * for all 3 remaining LEDs
		 */
		if (percent >= LED_2_THRESHOLD) {
			rgb_led_toggle(RGB_LEFT);
		}
		if (percent >= LED_3_THRESHOLD) {
			rgb_led_toggle(RGB_RIGHT);
		}
		if (percent >= LED_4_THRESHOLD) {
			rgb_led_toggle(RGB_DOWN);
		}

		test_mode_next_run += TEST_MODE_PERIOD_TICKS;
	    }
}
