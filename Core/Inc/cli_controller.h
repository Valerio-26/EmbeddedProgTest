#ifndef __CLI_CONTROLLER_H__
#define __CLI_CONTROLLER_H__

#include <stdint.h>

// Buffer per memorizzare il comando ricevuto
#define CLI_BUFFER_SIZE 100
extern char cli_buffer[CLI_BUFFER_SIZE];
extern uint8_t cli_index;
extern volatile char receivedChar;

// Funzione per inizializzare la CLI
void CLI_Init(void);

// Funzione per processare i comandi ricevuti
void CLI_ProcessCommand(char *command);

// Funzioni di callback per ogni comando
void CLI_Command_Raw(void);
void CLI_Command_Filter(void);
void CLI_Command_Noise(void);

#endif
