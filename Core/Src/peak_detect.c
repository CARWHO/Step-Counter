/*
 * peak_detect.c
 *
 *  Created on: May 6, 2025
 *      Author: una15, ohu15
 *
 *  implements a simple rising-edge peak detector on the squared magnitude of
 *  a 3-axis acceleration vector. This module:
 *    1. Computes mag2 = x*x + y*y + z*z (no sqrt).
 *    2. Compares mag2 against a pre-defined threshold (PEAK_DETECT_THRESHOLD).
 *    3. Detects a step when mag2 crosses from below to above the threshold.
 *    4. Increments an internal step_count on each detected rising edge.
 *  what each fucntion does:
 *    - peak_detect_init(): resets count and previous state.
 *    - peak_detect_update(x,y,z): updates state with new sample,
 *         returns true if a new peak was detected.
 *    - peak_detect_get_count(): returns total number of detected peaks.
 */

#include "peak_detect.h"
#include "imu_sensor.h"

/* For hysteresis, this value is chosen
 * after conducting live walking test
 */
#define SPS (400)


static uint32_t step_count;
static bool     prev_above;
static uint32_t samples_since_step;
static int64_t  current_mag2; // To store the latest squared magnitude

void peak_detect_init(void) {
    step_count = 0;
    imu_sensor_init();
    samples_since_step = SPS;

    /*
     * Fill the filters with the same values
     * so a peak is not detected on startup
     *
     * Calling create_imu_vector() because
     * sample average is also done inside of
     * this function
     */
    for (int i = 0; i < FILTER_TAPS; ++i) {
        (void)create_imu_vector();
    }

    // one more filtered sample and seed prev_above correctly:
    IMU_VECTOR v = create_imu_vector();
    int64_t mag2 = (int64_t)v.x*v.x
                 + (int64_t)v.y*v.y
                 + (int64_t)v.z*v.z;
    prev_above = (mag2 > (int64_t)PEAK_DETECT_THRESHOLD);
}


bool peak_detect_update(int32_t x, int32_t y, int32_t z) {

	/* Hysteresis for code sampling, taking sample
	 * when certain number of samples passed
	 */
	if (samples_since_step < SPS){
    samples_since_step++;
  }


  /*
   * Detecting a rising edge in the magnitude
   * of the imu vector higher than threshold
   * detected. This to detect a step by the
   * user
   */
  int64_t mag2 = (int64_t)x*x + (int64_t)y*y + (int64_t)z*z;
  current_mag2 = mag2; // Store the calculated squared magnitude
  bool    above    = (mag2 > (int64_t)PEAK_DETECT_THRESHOLD);
  bool    edge     = (!prev_above && above);


  /*
   * Counting step when not in hysteresis and
   * step detected
   */
  if (edge && samples_since_step >= SPS) {
    step_count++;
    samples_since_step = 0;      // restart cool‐down
  }

  prev_above = above;
  return edge && (samples_since_step == 0);
}


uint32_t peak_detect_get_count(void) {
    return step_count;
}

int64_t peak_detect_get_mag2(void) {
    return current_mag2;
}
