#ifndef __ANALOG_SIGNAL_CONTROLLER_H__
#define __ANALOG_SIGNAL_CONTROLLER_H__

#include "average_filter.h"
#include <stdlib.h>

typedef enum {RAW, FILTERED, AVERAGE_FILTERED} SignalType;

extern SignalType signal_type;
extern int random_noise;

int32_t get_analog_signal(int32_t new_value);

#endif