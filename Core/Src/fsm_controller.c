#include "fsm_controller.h"

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
extern uint32_t adc_value;

MovingAverageFilter adcFilter;
char msg[20];

void fsm_init(FsmController* fsm) {
    fsm->state = INIT;
}

void change_state(FsmController* fsm, State new_state) {
    fsm->state = new_state;
}

void button_pressed(FsmController* fsm) {
    switch (fsm->state) {
    case WAIT_REQUEST:
        change_state(fsm, LISTENING);
        break;
    case LISTENING:
        change_state(fsm, PAUSE);
        break;
    case PAUSE:
        change_state(fsm, LISTENING);
        break;
    case WARNING:
        change_state(fsm, LISTENING);
        break;
    default:
        change_state(fsm, FSMERROR);
        break;
    }
}

void fsm_run(FsmController* fsm) {
    switch (fsm->state) {
    case INIT:
        HAL_ADC_Start(&hadc1);
        initFilter(&adcFilter);
        change_state(fsm, WAIT_REQUEST);
        break;
    case WAIT_REQUEST:
        
        break;
    case LISTENING:
        HAL_ADC_PollForConversion(&hadc1, 20);
        adc_value = HAL_ADC_GetValue(&hadc1);
        updateFilter(&adcFilter, adc_value);
        sprintf(msg, "ADC: %lu\r\n", adc_value);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        break;
    case PAUSE:
        // lampeggia il led ogni 1000 ms
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);
        HAL_Delay(500);

        if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_13)){
            return change_state(fsm, LISTENING);
        }
        break;
    case WARNING:
        // Add logic for WARNING state
        break;
    case FSMERROR:
        // Add logic for ERROR state
        break;
    default:
        change_state(fsm, FSMERROR);
        break;
    }
}