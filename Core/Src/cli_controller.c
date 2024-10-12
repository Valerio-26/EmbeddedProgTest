#include "cli_controller.h"
#include "analog_signal_controller.h"
#include <stdio.h>
#include <string.h>
#include "main.h"

char cli_buffer[CLI_BUFFER_SIZE];
uint8_t cli_index = 0;
volatile char receivedChar;
extern UART_HandleTypeDef huart2;

int is_cli_active = 0;

// Funzione per inizializzare la CLI
void CLI_Init(void)
{
    memset(cli_buffer, 0, CLI_BUFFER_SIZE);

    //enable uart interrupt
    HAL_UART_Receive_IT(&huart2, (uint8_t*)&receivedChar, 1);
  __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
}

// Funzione per processare i comandi
void CLI_ProcessCommand(char *command)
{
    if(is_cli_active){
        if (strcmp(command, "raw") == 0)
            {
                CLI_Command_Raw();
            }
            else if (strcmp(command, "moving average") == 0)
            {
                CLI_Command_Filter();
            }
            else if (strcmp(command, "random noise") == 0)
            {
                CLI_Command_Noise();
            }
            else
            {
                HAL_UART_Transmit(&huart2, (uint8_t*)"Command not found!\r\n", strlen("Command not found!\r\n"), HAL_MAX_DELAY);
            }
    }
}

// Funzione per il comando "raw" (disabilita i filtri)
void CLI_Command_Raw(void)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)("comando raw ricevuto\r\n"), strlen("comando raw ricevuto\r\n"), HAL_MAX_DELAY);
    signal_type = RAW;
}

// Funzione per il comando "filter" (abilita un filtro)
void CLI_Command_Filter(void)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)("comando moving average ricevuto\r\n"), strlen("comando comand average ricevuto\r\n"), HAL_MAX_DELAY);
    signal_type = AVERAGE_FILTERED;
}

// Funzione per il comando "noise" (genera rumore randomico)
void CLI_Command_Noise(void)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)("comando random noise ricevuto\r\n"), strlen("comando random noise ricevuto\r\n"), HAL_MAX_DELAY);
    random_noise = !random_noise;
}

