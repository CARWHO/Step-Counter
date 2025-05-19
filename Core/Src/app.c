/*
 * app.c
 *
 *  Created on: Mar 10, 2025
 *      Author: Ohu15, Una14
 */

#include "app.h"
#include "button_task.h"
#include "joystick_task.h"
#include "display.h"
#include "counter_incrementer.h"
#include "test_mode.h"
#include "potentiometer.h"
#include "buzzer.h"
#include "state_selector.h"
#include "serial.h"
#include "imu_sensor.h"
#include "peak_detect.h"


#include "led_blinking.h"

void app_main(void)
{
    // Init each module
    button_task_init();
    joystick_task_init();
    potentiometer_task_init();
    display_init();
    counter_incrementer_init();
    test_mode_init();
    buzzer_init();

	led_init();

    serial_init();
    imu_sensor_init();
    peak_detect_init();

    while (1)
    {

    	// execute each module's logic
        button_task_execute();
    	led_execute();
        joystick_task_execute();
        display_execute();
        test_mode_execute();
        buzzer_execute();

        // read filtered IMU vector and detect peaks (steps)
        IMU_VECTOR v = create_imu_vector();
        if (peak_detect_update(v.x, v.y, v.z)) {
            // on detected step, increment counter
            counter_incrementer_increment_steps(1);
        }

        // tx serial data
        serial_task_execute();

    }
}
