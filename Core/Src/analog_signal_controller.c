#include "analog_signal_controller.h"

int32_t get_analog_signal(int32_t new_value){
    switch (signal_type) {
        case RAW:
            return new_value;
        case FILTERED:
            return 0;
        case AVERAGE_FILTERED:
            return moving_average_filter(new_value);
        case RANDOM_NOISE:
            return new_value + (rand() % 100) - 50;
        default:
            return new_value;
    }
}