#ifndef __ANALOG_SIGNAL_CONTROLLER_H__
#define __ANALOG_SIGNAL_CONTROLLER_H__

//#include "analog_filter.h"
#include "average_filter.h"
#include <stdlib.h>

typedef enum {RAW, FILTERED, AVERAGE_FILTERED, RANDOM_NOISE} SignalType;

//MovingAverageFilter filter;

int signal_type = FILTERED;

int32_t get_analog_signal(int32_t new_value);

#endif