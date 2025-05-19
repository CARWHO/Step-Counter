/*
 * peak_detect.h
 *
 *  Created on: May 6, 2025
 *      Author: una14, ohu15
 */

#ifndef PEAK_DETECT_H
#define PEAK_DETECT_H

#include <stdint.h>
#include <stdbool.h>


#ifndef PEAK_DETECT_THRESHOLD
#define PEAK_DETECT_THRESHOLD 855174784  // Approx 17.5 m/s^2: ((17.5 / 9.81) * 16393)^2
#endif

// Call once before first use
void    peak_detect_init(void);

// feed new filtered acc (x,y,z). returns true on a detected rising-edge peak.
bool    peak_detect_update(int32_t x, int32_t y, int32_t z);

// read total steps counted so far
uint32_t peak_detect_get_count(void);

// read the last computed squared magnitude from the peak detector
int64_t peak_detect_get_mag2(void);

#endif // PEAK_DETECT_H
