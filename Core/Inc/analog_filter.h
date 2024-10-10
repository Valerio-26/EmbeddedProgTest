#ifndef __ANALOG_FILTER_H__
#define __ANALOG_FILTER_H__

#define FILTER_SIZE 150
#include <stdint.h>
#include <string.h>

typedef struct {
    int32_t buffer[FILTER_SIZE];
    int32_t sum;
    uint8_t index;
    uint8_t count;
} MovingAverageFilter;

void initFilter(MovingAverageFilter* filter);
int32_t updateFilter(MovingAverageFilter* filter, int32_t newValue);


#endif