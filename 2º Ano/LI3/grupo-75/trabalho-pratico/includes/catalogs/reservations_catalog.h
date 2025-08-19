#ifndef RESERVATIONS_CATALOG_H
#define RESERVATIONS_CATALOG_H


#include <glib.h>

#include "types/reservation.h"

// HashTable dos reservas, reservations é um apontador para a hashtable, 
//e RESERVATIONS_CATALOG é um type que representa um apontador para a struct.
typedef struct reservations_catalog 
{
    GHashTable *reservations;

} *RESERVATIONS_CATALOG;

RESERVATIONS_CATALOG create_reservations_catalog(void);

void insert_reservation(RESERVATIONS_CATALOG catalog, RESERVATION reservation);

RESERVATION get_reservation_by_id(RESERVATIONS_CATALOG catalog, char* reservation_id);

void count_user_reservations_and_total_spent(RESERVATIONS_CATALOG catalog, char* user_id, int* count, double* total_spent);

double hotel_average_rating(RESERVATIONS_CATALOG catalog, char* hotel_id);

void free_reservations_catalog(RESERVATIONS_CATALOG catalog);

#endif