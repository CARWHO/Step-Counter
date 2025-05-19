/*
 * led_blinking.h
 *
 *  Created on: May 14, 2025
 *      Author: una14, ohu15
 */

#ifndef INC_LED_BLINKING_H_
#define INC_LED_BLINKING_H_

#include <stdint.h>
#include "rgb.h"

#define NUM_THRESHOLDS 4


// Initialising function for the module
void led_init(void);

// Execute function for the module
void led_execute(void);

#endif /* INC_LED_BLINKING_H_ */
