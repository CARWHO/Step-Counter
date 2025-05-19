/*
 * imu_sensor.h
 *
 *  Created on: May 6, 2025
 *      Author: una14, ohu15
 */

#ifndef __IMU_SENSOR_H__
#define __IMU_SENSOR_H__

#include <stdint.h>
#include "noise_filter.h"


// Struct for imu data
typedef struct {
    int32_t x;    // filtered, signed 32-bit
    int32_t y;
    int32_t z;
} IMU_VECTOR;


// Getting
IMU_VECTOR get_raw_filter(void);
void imu_sensor_init(void);

// Creates and filters using FIR in buffer
IMU_VECTOR create_imu_vector(void);

#endif /* __IMU_SENSOR_H__ */
