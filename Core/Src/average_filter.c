#include "average_filter.h"

int32_t sensor_values[WINDOW_SIZE] = {0};  // Buffer per memorizzare i valori
int average_index = 0;                     // Indice corrente nel buffer
int32_t sum = 0;                           // Somma dei valori nella finestra

int32_t moving_average_filter(int32_t new_value) {
    // Rimuovi il valore più vecchio dalla somma
    sum -= sensor_values[average_index];

    // Aggiungi il nuovo valore al buffer e alla somma
    sensor_values[average_index] = new_value;
    sum += new_value;

    // Aggiorna l'indice
    average_index = (average_index + 1) % WINDOW_SIZE;

    // Calcola e restituisci la media mobile
    return sum / WINDOW_SIZE;
}