/*
 * potentiometer.c
 *
 *  Created on: Mar 12, 2025
 *      Author: Ohu15, Una14
 */

#include "button_task.h"
#include "buttons.h"
#include "rgb.h"
#include "stm32c0xx_hal.h"
#include "tim.h"
#include "pwm.h"
#include "joystick_task.h"
#include "usart.h"
#include "display.h"
#include "counter_incrementer.h"
#include "potentiometer.h"
#include "test_mode.h"

#include <string.h>

// For scheduling
#define BUTTON_POLL_FREQ       500
#define TICK_FREQUENCY_HZ      1000
#define BUTTON_PERIOD_TICKS    (TICK_FREQUENCY_HZ / BUTTON_POLL_FREQ)
#define INC_DUTYCYCLE          10
#define TIME_OUT_VALUE         50

#define DOUBLE_PRESS_THRESHOLD_MS 300
#define DEBOUNCE_DELAY_MS 50

char uart_buffer[100];

static uint32_t buttonNextRun = 0;
static uint32_t Duty_cycle = 0;

static bool prev_up_pressed = false;
static uint32_t last_up_press_time = 0;
static bool waiting_for_double_press = false;
static uint32_t last_valid_up_press_time = 0;

uint8_t UART_button_state = 0;

void uart_transmit_button(buttonState_t downState) {

    static bool prev_down_pressed = false;
    static uint16_t down_count = 0;
    bool current_down_pressed = (downState == PUSHED);

    if (current_down_pressed)
    {
        if (prev_down_pressed)
        {
            down_count++;
            if (down_count >= NUM_BUT_POLLS_1)
            {
                UART_button_state = (UART_button_state == 0) ? 1 : 0;
                down_count = 0;
            }
        }
        else
        {
            UART_button_state = (UART_button_state == 0) ? 1 : 0;
            down_count = 0;
        }
    }
    else
    {
        down_count = 0;
    }
    prev_down_pressed = current_down_pressed;

    if (UART_button_state == 1)
    {
        snprintf(uart_buffer, sizeof(uart_buffer),
                 "\n%d, %d, %d, %d, %ld, %ld km, %ld yd, %ld, %d, %d\n",
                 joystick_X_val(),
                 joystick_Y_val(),
				 joystick_click_pressed(),
                 state_getter(),
                 counter_incrementer_get_steps(),
                 counter_incrementer_get_distance()[0], //km
				 counter_incrementer_get_distance()[1], //yards
                 counter_incrementer_get_goal(),
				 joystick_click_pressed(),
                 potentiometer_val()

				 );
        HAL_UART_Transmit(&huart2, (uint8_t*)uart_buffer,
                          strlen(uart_buffer), TIME_OUT_VALUE);
    }
}

void button_task_init(void)
{
    buttons_init();
    buttonNextRun = HAL_GetTick() + BUTTON_PERIOD_TICKS;
    pwm_setDutyCycle(&htim2, TIM_CHANNEL_3, Duty_cycle);
}

void test_mode_button_check(void) {

    // SW2 (UP button) double press detection for toggling test mode with debounce:
    uint32_t now = HAL_GetTick();
    bool current_up_pressed = (buttons_checkButton(DOWN) == PUSHED);
    if (current_up_pressed && !prev_up_pressed) {
        // Only consider this a valid press if outside the debounce period.
        if ((now - last_valid_up_press_time) >= DEBOUNCE_DELAY_MS) {
            // Register this as a valid press.
            last_valid_up_press_time = now;
            if (!waiting_for_double_press) {
                // First valid press detected: start the double press timer.
                waiting_for_double_press = true;
                last_up_press_time = now;
            } else {
                // Second valid press detected: check if within the threshold.
                if ((now - last_up_press_time) <= DOUBLE_PRESS_THRESHOLD_MS) {
                    test_mode_toggle();
                    waiting_for_double_press = false;
                } else {
                    // Too late; reset and treat this as a new first press.
                    last_up_press_time = now;
                }
            }
        }
    }
    prev_up_pressed = current_up_pressed;
    // Reset waiting_for_double_press if the time threshold has passed without a second valid press.
    if (waiting_for_double_press && ((now - last_up_press_time) > DOUBLE_PRESS_THRESHOLD_MS)) {
        waiting_for_double_press = false;
    }

}

void button_task_execute(void)
{
    uint32_t now = HAL_GetTick();
    if (now >= buttonNextRun)
    {
        buttons_update();

        test_mode_button_check();

        // Process DOWN button for LED toggling, UART transmit logic, and step increment of 80
        buttonState_t downState = buttons_checkButton(UP);
        static bool prev_down_pressed_for_increment = false;
        if (downState == PUSHED)
        {
            rgb_led_toggle(RGB_DOWN);
            rgb_colour_all_on();
            if (!prev_down_pressed_for_increment) {
                counter_incrementer_increment_steps(80);
            }
            prev_down_pressed_for_increment = true;
        }
        else
        {
            prev_down_pressed_for_increment = false;
        }


        // LEFT / RIGHT button handling remains unchanged:
        if (buttons_checkButton(LEFT) == PUSHED)
        {
            rgb_led_toggle(RGB_LEFT);
            rgb_colour_all_on();
        }
        if (buttons_checkButton(RIGHT) == PUSHED)
        {
            rgb_led_toggle(RGB_RIGHT);
            rgb_colour_all_on();
        }

        // Always toggle an LED for additional feedback
        rgb_led_toggle(RGB_UP);
        rgb_colour_all_on();

        uart_transmit_button(downState);

        buttonNextRun += BUTTON_PERIOD_TICKS;
    }
}
