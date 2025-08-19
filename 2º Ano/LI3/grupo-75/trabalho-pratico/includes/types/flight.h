#ifndef FLIGHT_H
#define FLIGHT_H

#include <glib.h>

typedef struct flight
{
    char *id;
    char *airline;
    char *plane_model;
    int total_seats;
    char *origin;
    char *destination;
    char *schedule_departure;
    char *schedule_arrival;
    char *real_departure;
    char *real_arrival;
    char *pilot;
    char *copilot;
    char *notes;

    GHashTable *passengers;
    int delay;
} *FLIGHT;

void add_flight_passenger(FLIGHT flight, char *user_id);

FLIGHT build_flight(char **flight_params);

void free_flight(FLIGHT flight);

int count_passengers(FLIGHT flight);

#endif