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

#define LINE_WIDTH 21

static uint32_t displayNextRun = 0;



void display_init(void) {
    displayNextRun = HAL_GetTick() + BLINKY_PERIOD_TICKS;
    ssd1306_Init();
}





void display_title( char* title) {

	ssd1306_Fill(Black);
    ssd1306_SetCursor(5, 0);

    ssd1306_WriteString(title, Font_7x10, White);
}


void format_display(int x, int y, const char* display_char, uint32_t input1) {

    char stepBuf[32];
    char paddedStep[50];

    snprintf(stepBuf, sizeof(stepBuf), display_char, input1);
    pad_string(paddedStep, sizeof(paddedStep), stepBuf, 15);

    ssd1306_SetCursor(x, y);
    ssd1306_WriteString(paddedStep, Font_7x10, White);

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


/*
 * Check if test mode conditions have been met
 * Then activate test mode
 */
void activate_test_mode(void){

	if (test_mode_is_active()) {
        ssd1306_SetCursor(0, 56);
        ssd1306_WriteString("TEST", Font_7x10, White);
    }

}


void display_execute(void) {
    uint32_t now = HAL_GetTick();
    if (now >= displayNextRun) {
        state_display(get_xy_values());
        ssd1306_UpdateScreen();

        activate_test_mode();

        // If test mode is active, display "TEST" at the bottom of the screen.

        ssd1306_UpdateScreen();
        displayNextRun += BLINKY_PERIOD_TICKS;
    }
}
