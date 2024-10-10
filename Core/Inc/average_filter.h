#ifndef __AVERAGE_FILTER_H__
#define __AVERAGE_FILTER_H__

#define WINDOW_SIZE 150

#include <stdint.h>

extern int32_t sensor_values[WINDOW_SIZE];  // Buffer per memorizzare i valori
extern int average_index;                  // Indice corrente nel buffer
extern int32_t sum;                        // Somma dei valori nella finestra

// Funzione per aggiungere un nuovo valore e ottenere la media mobile
int32_t moving_average_filter(int32_t new_value);

#endif // __AVERAGE_FILTER_H__