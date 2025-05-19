/*
 * noise_filter.h
 *
 *  Created on: May 6, 2025
 *      Author: una14
 */

#ifndef INC_NOISE_FILTER_H_
#define INC_NOISE_FILTER_H_

#include <stdint.h>
#include <stddef.h>

// Number of samples for the moving-average filter
#define FILTER_TAPS  10

// Simple averaging filter struct: holds past samples and write index
typedef struct {
    int32_t buf[FILTER_TAPS];  // circular buffer of recent samples
    size_t  idx;               // next write position in the buffer
} avg_filter_t;

// Initialize the filter: zero buffer and reset index
void    avg_filter_init(avg_filter_t *f);

// Push a new sample, update internal buffer, and return the averaged result
int32_t avg_filter_update(avg_filter_t *f, int32_t new_sample);

#endif /* INC_NOISE_FILTER_H_ */
