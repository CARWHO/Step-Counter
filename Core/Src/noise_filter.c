/*
 * noise_filter.c
 *
 *  Created on: May 6, 2025
 *      Author: una14, ohu15
 *
 *  implements a simple moving-average (fir) low-pass filter. The filter struct
 *  (avg_filter_t) maintains a circular buffer of the last FILTER_TAPS samples and
 *  an index pointing to the next write position. Each time avg_filter_update() is
 *  called:
 *    1. The new sample is stored at buf[idx].
 *    2. The index is incremented (wrapping around when it reaches FILTER_TAPS).
 *    3. All samples in the buffer are summed and divided by FILTER_TAPS,
 *       producing the average (smoothed output).
 *  calling avg_filter_init() zeroes the buffer and resets idx to 0, ensuring the
 *  filter starts in a known state with no residual data.
 */

#include "noise_filter.h"
#include <stddef.h>
#include <stdint.h>

void avg_filter_init(avg_filter_t *f) {
    for (size_t i = 0; i < FILTER_TAPS; ++i) {
        f->buf[i] = 0;
    }
    // start index @ 0
    f->idx = 0;
}

int32_t avg_filter_update(avg_filter_t *f, int32_t new_sample) {
    f->buf[f->idx] = new_sample;
    f->idx = (f->idx + 1) % FILTER_TAPS;

    // sum of buffer
    int64_t sum = 0;
    for (size_t i = 0; i < FILTER_TAPS; ++i) {
        sum += f->buf[i];
    }
    // average
    return (int32_t)(sum / FILTER_TAPS);
}
