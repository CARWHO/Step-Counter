// buzzer.h
#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32c0xx_hal.h"
#include "pwm.h"

/**
 * @brief  Initialise the buzzer (must be called after MX_TIM16_Init).
 *         Configures TIM16_CH1 in OC-Toggle mode and ensures the line is low.
 */
void buzzer_init(void);

/**
 * @brief  Start a continuous square-wave on PD0.
 */
void buzzer_on(void);

/**
 * @brief  Stop the square-wave and drive PD0 low.
 */
void buzzer_off(void);

/**
 * @brief  Buzz for a fixed number of milliseconds.
 * @param  ms  Duration in ms.
 */
void buzzer_play_ms(uint32_t ms);

/**
 * @brief  Call periodically (e.g. in your main loop) to handle
 *         stopping after buzzer_play_ms().
 */
void buzzer_execute(void);

#endif // __BUZZER_H
