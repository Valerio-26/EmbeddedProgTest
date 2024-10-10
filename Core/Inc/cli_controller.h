#ifndef __CLI_CONTROLLER_H__
#define __CLI_CONTROLLER_H__

#include <stdint.h>

// Funzione per inizializzare la CLI
void CLI_Init(void);

// Funzione per processare i comandi ricevuti
void CLI_ProcessCommand(char *command);

// Funzioni di callback per ogni comando
void CLI_Command_Raw(void);
void CLI_Command_Filter(void);
void CLI_Command_Noise(void);

#endif
