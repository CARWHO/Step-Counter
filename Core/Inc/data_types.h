/*
* data_types.h
*
* Created on: Apr 3, 2025
* Author: una14
*/

#ifndef SRC_DATA_TYPES_H_
#define SRC_DATA_TYPES_H_

#include <stdbool.h>


// Common MACRO's used across the program

#define STEPS_TO_YARDS(STEPS) ((STEPS * 9 * 1094) / 10000)
#define STEPS_TO_KM_INTEGER(STEPS) ((STEPS * 9) / 10000)
#define CONV_ADC_TO_GOAL(ADC_VALUE) ((ADC_VALUE * 15250UL) / 4096)
#define INTEGER_ARITHMETHIC_250(NUM) ((NUM / 250) * 250)

#define PERIOD_ONE_SECOND_IN_MS 1000



#define X_CENTER     2284
#define X_FULL_LEFT  337    // Maps to -100%
#define X_FULL_RIGHT 4095   // Maps to +100%

#define Y_CENTER     2253
#define Y_FULL_UP    455   // Maps to +100%
#define Y_FULL_DOWN  3945   // Maps to -100%

#define VALUE_5 	5
#define VALUE_10 	10
#define VALUE_90 	90
#define VALUE_100 	100







// for testing
extern uint16_t raw_adc[3];

typedef enum {
    STATE_STATE_CURRENT_STEP = 0,
    STATE_GOAL_PROGRESS,
    STATE_DIST_TRAVELLED,
    STATE_GOAL_EDIT
} State;

// Removed the static state variable from here to manage it in state_selector.c

typedef struct {
    int16_t xVal;
    int16_t yVal;
    int16_t xPercent;
    int16_t yPercent;
    const char* x_state;
    const char* y_state;
} xy_display;

typedef struct {
    uint16_t step_goal;
    uint16_t step_travelled;
    uint16_t step_dist_km;
    bool flag_unit; // 0 : km, 1: yards
} step_data;

#endif /* SRC_DATA_TYPES_H_ */
