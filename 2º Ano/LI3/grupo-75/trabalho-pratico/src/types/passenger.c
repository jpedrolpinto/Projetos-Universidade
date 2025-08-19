#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types/passenger.h"
#include "types/user.h"
#include "types/flight.h"
#include "utilities/data_handler.h"

PASSENGER create_passenger()
{
    PASSENGER new_passenger = malloc(sizeof(struct passenger));
    new_passenger->flight_id = NULL;
    new_passenger->user_id = NULL;

    return new_passenger;
}

void set_passenger_flight_id(PASSENGER passenger, char *passenger_flight_id)
{
    passenger->flight_id = strdup(passenger_flight_id);
}

void set_passenger_user_id(PASSENGER passenger, char *passenger_user_id)
{
    passenger->user_id = strdup(passenger_user_id);
}

int verify_passenger(char **parameters)
{
    // Verify that both user and flight IDs aren't NULL or empty
    if (parameters[0] && strlen(parameters[0]) &&
        parameters[1] && strlen(parameters[1]))
        return 1;

    return 0;
}

int process_passenger(char **passenger_params, USERS_CATALOG users_catalog, FLIGHTS_CATALOG flights_catalog)
{
    if (!verify_passenger(passenger_params))
        return 0;

    char *flight_id = passenger_params[0];
    char *user_id = passenger_params[1];

    FLIGHT flight = get_flight_by_id(flights_catalog, flight_id);
    if (!flight)
        return 0;

    USER user = get_user_by_id(users_catalog, user_id);
    if (!user)
        return 0;

    add_flight_passenger(flight, user->id);
    add_user_flight(user, flight->id, flights_catalog);

    return 1;
}

void free_passenger(PASSENGER passenger)
{
    if (passenger->flight_id != NULL)
        free(passenger->flight_id);
    if (passenger->user_id != NULL)
        free(passenger->user_id);

    free(passenger);
}