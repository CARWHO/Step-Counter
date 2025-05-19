/*
 * imu_sensor.c
 *
 *  Created on: May 6, 2025
 *      Author: una14, ohu15
 */

#include "imu_sensor.h"
#include "imu_lsm6ds.h"
#include "noise_filter.h"
#include <stdint.h>

// moving-average filters, one per axis
static avg_filter_t filt_x, filt_y, filt_z;

// helper to read raw 16-bit signed accel from LSM6DS
static int16_t read_raw_axis(uint8_t low_reg, uint8_t high_reg) {
    uint8_t lo = imu_lsm6ds_read_byte(low_reg);
    uint8_t hi = imu_lsm6ds_read_byte(high_reg);
    return (int16_t)((hi << 8) | lo);
}

void imu_sensor_init(void) {
    // turn on accelerometer, high-performance
    imu_lsm6ds_write_byte(CTRL1_XL, CTRL1_XL_HIGH_PERFORMANCE);

    // init each filter
    avg_filter_init(&filt_x);
    avg_filter_init(&filt_y);
    avg_filter_init(&filt_z);
}

IMU_VECTOR create_imu_vector(void) {
    // get raw readings
    int16_t raw_x = read_raw_axis(OUTX_L_XL, OUTX_H_XL);
    int16_t raw_y = read_raw_axis(OUTY_L_XL, OUTY_H_XL);
    int16_t raw_z = read_raw_axis(OUTZ_L_XL, OUTZ_H_XL);

    // apply moving-average filter
    int32_t fx = avg_filter_update(&filt_x, raw_x);
    int32_t fy = avg_filter_update(&filt_y, raw_y);
    int32_t fz = avg_filter_update(&filt_z, raw_z);

    // return filtered vector
    IMU_VECTOR v = { fx, fy, fz };
    return v;
}

IMU_VECTOR get_raw_filter(void) {
    int16_t raw_x = read_raw_axis(OUTX_L_XL, OUTX_H_XL);
    int16_t raw_y = read_raw_axis(OUTY_L_XL, OUTY_H_XL);
    int16_t raw_z = read_raw_axis(OUTZ_L_XL, OUTZ_H_XL);

    // return filtered vector
    IMU_VECTOR v = { raw_x, raw_y, raw_z };
    return v;


}
