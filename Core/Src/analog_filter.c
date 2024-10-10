#include "analog_filter.h"

void initFilter(MovingAverageFilter* filter) {
    memset(filter->buffer, 0, sizeof(filter->buffer));
    filter->sum = 0;
    filter->index = 0;
    filter->count = 0;
}

int32_t updateFilter(MovingAverageFilter* filter, int32_t newValue) {
    // Subtract the oldest value from the sum
    if (filter->count == FILTER_SIZE) {
        filter->sum -= filter->buffer[filter->index];
    }

    // Add the new value
    filter->buffer[filter->index] = newValue;
    filter->sum += newValue;

    // Update index and count
    filter->index = (filter->index + 1) % FILTER_SIZE;
    if (filter->count < FILTER_SIZE) {
        filter->count++;
    }

    // Calculate and return the average
    return filter->sum / filter->count;
}