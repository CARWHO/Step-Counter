// buzzer.c
#include "buzzer.h"
#include "tim.h"
#include "stm32c0xx_hal.h"
#include "counter_incrementer.h"
#include <stdbool.h>
#include <stdint.h>



#define BUZZER_POLL_FREQ       2
#define TICK_FREQUENCY_HZ      1000
#define BUZZER_PERIOD_TICKS    (TICK_FREQUENCY_HZ / BUZZER_POLL_FREQ)

static uint32_t buzzerNextRun = 0;


static uint32_t buzzer_stop_tick = 0;
static bool     last_goal_met   = false;

void buzzer_init(void)
{
    // Ensure timer is stopped and pin is low
    HAL_TIM_OC_Stop(&htim16, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);

    // Reset state
    buzzer_stop_tick = 0;
    last_goal_met    = false;
}

void buzzer_on(void)
{
    HAL_TIM_OC_Start(&htim16, TIM_CHANNEL_1);
}

void buzzer_off(void)
{
    HAL_TIM_OC_Stop(&htim16, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET);
}

void buzzer_play_ms(uint32_t ms)
{
    buzzer_stop_tick = HAL_GetTick() + ms;
    buzzer_on();
}

void buzzer_execute(void)
{
    uint32_t now      = HAL_GetTick();

    if (now >= buzzerNextRun)
	{
		bool     goal_met = (counter_incrementer_get_steps() >= counter_incrementer_get_goal());

		// On rising edge of goal condition, start a 1 s buzz
		if (goal_met && !last_goal_met)
		{
			buzzer_play_ms(1000);
		}
		last_goal_met = goal_met;

		// Stop the buzzer once the time expires
		if (buzzer_stop_tick && (int32_t)(now - buzzer_stop_tick) >= 0)
		{
			buzzer_off();
			buzzer_stop_tick = 0;

			}
		buzzerNextRun += BUZZER_PERIOD_TICKS;
        }
}
