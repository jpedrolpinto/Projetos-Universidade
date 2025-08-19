#ifndef FLIGHTS_CATALOG_H
#define FLIGHTS_CATALOG_H

#include <glib.h>

#include "types/flight.h"

// HashTable dos voos, flights é um apontador para a hashtable,
// e FLIGHTS_CATALOG é um type que representa um apontador para a struct.
typedef struct flights_catalog
{
    GHashTable *flights;
} *FLIGHTS_CATALOG;

FLIGHTS_CATALOG create_flights_catalog(void);

void insert_flight(FLIGHTS_CATALOG catalog, FLIGHT flight);

FLIGHT get_flight_by_id(FLIGHTS_CATALOG catalog, char *flight_id);

double airport_delays(FLIGHTS_CATALOG catalog, char *origin);

void free_flights_catalog(FLIGHTS_CATALOG catalog);

#endif