/*
 * app.c
 *
 *  Created on: Mar 10, 2025
 *      Author: Ohu15, Una14
 */

#include "app.h"
#include "blinky.h"
#include "button_task.h"
#include "joystick_task.h"
#include "display.h"
#include "counter_incrementer.h"
#include "test_mode.h"  // New include for test mode
#include "potentiometer.h"
#include "buzzer.h"
#include "state_selector.h"

void app_main(void)
{
    // Init each module
    blinky_init();
    button_task_init();
    joystick_task_init();
    potentiometer_task_init();
    display_init();
    counter_incrementer_init();
    test_mode_init();
    buzzer_init();

    while (1)
    {
        // Execute each modules logic
        blinky_execute();
        button_task_execute();
        joystick_task_execute();
        display_execute();
        test_mode_execute();
        buzzer_execute();

    }
}
