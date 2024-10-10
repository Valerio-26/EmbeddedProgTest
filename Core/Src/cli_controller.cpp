#include "cli_controller.h"
#include <stdio.h>
#include <string.h>

// Buffer per memorizzare il comando ricevuto
#define CLI_BUFFER_SIZE 100
char cli_buffer[CLI_BUFFER_SIZE];

// Funzione per inizializzare la CLI
void CLI_Init(void)
{
    memset(cli_buffer, 0, CLI_BUFFER_SIZE); // Inizializza il buffer
    printf("CLI Ready. Type 'raw', 'filter' or 'noise'.\n");
}

// Funzione per processare i comandi
void CLI_ProcessCommand(char *command)
{
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
        printf("Unknown command: %s\n", command);
    }
}

// Funzione per il comando "raw" (disabilita i filtri)
void CLI_Command_Raw(void)
{
    printf("Raw mode activated. Filters disabled.\n");
    // Disabilita eventuali filtri applicati ai dati
    // Codice per disabilitare i filtri
}

// Funzione per il comando "filter" (abilita un filtro)
void CLI_Command_Filter(void)
{
    printf("Filter mode activated. Applying filters.\n");
    // Abilita un filtro ai dati del sensore
    // Codice per abilitare il filtro
}

// Funzione per il comando "noise" (genera rumore randomico)
void CLI_Command_Noise(void)
{
    printf("Noise mode activated. Generating random noise.\n");
    // Codice per generare rumore randomico
}

