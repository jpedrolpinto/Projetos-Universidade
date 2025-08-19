#ifndef PASSENGER_H
#define PASSENGER_H

#include "catalogs/users_catalog.h"
#include "catalogs/flights_catalog.h"

typedef struct passenger
{
    char *flight_id;
    char *user_id;

} *PASSENGER;

int process_passenger(char **passenger_params, USERS_CATALOG users_catalog, FLIGHTS_CATALOG flights_catalog);

void free_passenger(PASSENGER passenger);

#endif