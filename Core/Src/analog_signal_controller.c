#include "analog_signal_controller.h"

SignalType signal_type = RAW;
int random_noise = 0;

int32_t get_analog_signal(int32_t new_value){
    if(random_noise){
        new_value += (rand() % 100) - 50;
        if (new_value < 0) {
            new_value = 0;
        }
    }
    switch (signal_type) {
        case RAW:
            return new_value;
        case FILTERED:
            return 0;
        case AVERAGE_FILTERED:
            return moving_average_filter(new_value);
        default:
            return new_value;
    }
}