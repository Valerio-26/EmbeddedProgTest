#include "fsm_controller.h"
#include "analog_signal_controller.h"
#include "cli_controller.h"
#include <stdlib.h>
#include "main.h" // Include the header file that defines LD2_GPIO_Port and LD2_Pin

#define ADC_RESOLUTION 12;
#define ADC_MAX_VALUE ((1 << ADC_RESOLUTION) - 1) // 4095 for 12-bit ADC
#define VREF 5 // volts
#define BUFFER_SIZE 1

extern ADC_HandleTypeDef hadc1;
extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern int is_magnet_detected;
extern int is_cli_active;
extern int is_sensor_active;

uint32_t hall_sensor_time = 0;
uint32_t adc_value = 0;
uint16_t adc_buffer[BUFFER_SIZE];
volatile uint8_t conversion_complete = 0;
char msg[20];

void fsm_init(FsmController* fsm) {
    fsm->state = INIT;
}

void change_state(FsmController* fsm, State new_state) {
    fsm->state = new_state;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
    if(hadc->Instance == ADC1){
       conversion_complete = 1;
    }
    HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, BUFFER_SIZE);
}

void ProcessADCData(){
    for(int i = 0; i < BUFFER_SIZE; ++i){
        uint16_t rawValue = adc_buffer[0];
        sprintf(msg, "Analog: %d\r\n", rawValue);
        HAL_UART_Transmit(&huart2, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
    }
    conversion_complete = 0;
}

void button_pressed(FsmController* fsm) {
    switch (fsm->state) {
    case WAIT_REQUEST:
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        is_cli_active = 0;
        is_sensor_active = 1;
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, BUFFER_SIZE);
        change_state(fsm, LISTENING);
        break;
    case LISTENING:
        change_state(fsm, PAUSE);
        HAL_TIM_Base_Start_IT(&htim2);
        is_cli_active = 1;
        is_sensor_active = 0;
        HAL_ADC_Stop_DMA(&hadc1);
        HAL_UART_Transmit(&huart2, (uint8_t*)"PAUSE\r\n", strlen("PAUSE\r\n"), HAL_MAX_DELAY);
        break;
    case PAUSE:
        change_state(fsm, LISTENING);
        HAL_TIM_Base_Stop_IT(&htim2);
        is_cli_active = 0;
        is_sensor_active = 1;
        HAL_ADC_Start_DMA(&hadc1, (uint32_t*)adc_buffer, BUFFER_SIZE);
        break;
    case WARNING:
        is_cli_active = 1;
        HAL_UART_Transmit(&huart2, (uint8_t*)"WAIT_REQUEST\r\n", strlen("WAIT_REQUEST\r\n"), HAL_MAX_DELAY);
        change_state(fsm, WAIT_REQUEST);
        break;
    default:
        change_state(fsm, FSMERROR);
        NVIC_SystemReset();
        break;
    }
}

void fsm_run(FsmController* fsm) {
    switch (fsm->state) {
    case INIT:
        srand(HAL_GetTick());
        CLI_Init();
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        is_cli_active = 1;
        HAL_UART_Transmit(&huart2, (uint8_t*)"WAIT_REQUEST\r\n", strlen("WAIT_REQUEST\r\n"), HAL_MAX_DELAY);
        change_state(fsm, WAIT_REQUEST);
        break;
    case WAIT_REQUEST:
        
        break;
    case LISTENING:
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
        if(conversion_complete){
            ProcessADCData();
        }
        
        HAL_Delay(100);

        break;
    case PAUSE:
        break;
    case WARNING:
        // Add logic for WARNING state
        is_cli_active = 0;
        is_sensor_active = 0;
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
        HAL_UART_Transmit(&huart2, (uint8_t*)"WARNING\r\n", strlen("WARNING\r\n"), HAL_MAX_DELAY);
        HAL_Delay(100);
        break;
    case FSMERROR:
        is_cli_active = 0;
        is_sensor_active = 0;
        HAL_ADC_Stop_DMA(&hadc1);
        HAL_TIM_Base_Start_IT(&htim3);
        HAL_UART_Transmit(&huart2, (uint8_t*)"ERROR\r\n", strlen("ERROR\r\n"), HAL_MAX_DELAY);
        HAL_Delay(100);
        break;
    default:
        change_state(fsm, FSMERROR);
        break;
    }
}