#include "fsm_controller.h"
#include "main.h" // Include the header file that defines LD2_GPIO_Port and LD2_Pin

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
extern uint32_t adc_value;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern int is_magnet_detected;

MovingAverageFilter adcFilter;
uint32_t hall_sensor_time = 0;
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
        HAL_TIM_Base_Start_IT(&htim2);
        HAL_UART_Transmit(&huart2, (uint8_t*)"PAUSE\r\n", strlen("PAUSE\r\n"), HAL_MAX_DELAY);
        break;
    case PAUSE:
        change_state(fsm, LISTENING);
        HAL_TIM_Base_Stop_IT(&htim2);
        break;
    case WARNING:
        change_state(fsm, LISTENING);
        break;
    default:
        change_state(fsm, FSMERROR);
        NVIC_SystemReset(); //assicurarsi che il timer 3 venga spento
        break;
    }
}

void fsm_run(FsmController* fsm) {
    switch (fsm->state) {
    case INIT:
        initFilter(&adcFilter);
        change_state(fsm, WAIT_REQUEST);
        break;
    case WAIT_REQUEST:
        //HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        HAL_UART_Transmit(&huart2, (uint8_t*)"WAIT_REQUEST\r\n", strlen("WAIT_REQUEST\r\n"), HAL_MAX_DELAY);
        HAL_Delay(1000);
        break;
    case LISTENING:
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        if(is_magnet_detected){
                if(hall_sensor_time == 0){
                    hall_sensor_time = HAL_GetTick();
                }else{
                    if(HAL_GetTick() - hall_sensor_time >= 5000){
                        change_state(fsm, WARNING);
                        hall_sensor_time = 0;
                    }
                }
        }else{
            hall_sensor_time = 0;
        }
        HAL_ADC_Start(&hadc1);
        HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);
        adc_value = HAL_ADC_GetValue(&hadc1);
        //int32_t filtered_value = updateFilter(&adcFilter, adc_value);
        sprintf(msg, "Analog: %lu %ld\r\n", adc_value, hall_sensor_time);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
        HAL_Delay(100);

        break;
    case PAUSE:
        break;
    case WARNING:
        // Add logic for WARNING state
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        HAL_UART_Transmit(&huart2, (uint8_t*)"WARNING\r\n", strlen("WARNING\r\n"), HAL_MAX_DELAY);
        HAL_Delay(100);
        break;
    case FSMERROR:
        HAL_TIM_Base_Start_IT(&htim3);
        HAL_UART_Transmit(&huart2, (uint8_t*)"ERROR\r\n", strlen("ERROR\r\n"), HAL_MAX_DELAY);
        HAL_Delay(100);
        break;
    default:
        change_state(fsm, FSMERROR);
        break;
    }
}