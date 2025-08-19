#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types/flight.h"
#include "types/reservation.h"
#include "utilities/data_handler.h"
#include "utilities/date_time.h"

FLIGHT create_flight()
{

    FLIGHT new_flight = malloc(sizeof(struct flight));
    new_flight->id = NULL;
    new_flight->airline = NULL;
    new_flight->plane_model = NULL;
    new_flight->origin = NULL;
    new_flight->destination = NULL;
    new_flight->pilot = NULL;
    new_flight->copilot = NULL;
    new_flight->notes = NULL;

    new_flight->passengers = g_hash_table_new(g_str_hash, g_str_equal); // Hash Table that contains the passengers of each flight 

    return new_flight;
}

int count_passengers(FLIGHT flight)
{
    return g_hash_table_size(flight->passengers);
}

void set_flight_id(FLIGHT flight, char *flight_id)
{
    flight->id = strdup(flight_id);
}

void set_flight_airline(FLIGHT flight, char *flight_airline)
{
    flight->airline = strdup(flight_airline);
}

void set_flight_plane_model(FLIGHT flight, char *flight_plane_model)
{
    flight->plane_model = strdup(flight_plane_model);
}

void set_flight_total_seats(FLIGHT flight, char *flight_total_seats)
{
    flight->total_seats = string_to_int(flight_total_seats);
}

void set_flight_origin(FLIGHT flight, char *flight_origin)
{
    to_upper(flight_origin);
    flight->origin = strdup(flight_origin);
}

void set_flight_destination(FLIGHT flight, char *flight_destination)
{
    to_upper(flight_destination);
    flight->destination = strdup(flight_destination);
}

void set_flight_schedule_departure(FLIGHT flight, char *flight_schedule_departure)
{
    flight->schedule_departure = strdup(flight_schedule_departure);
}

void set_flight_schedule_arrival(FLIGHT flight, char *flight_schedule_arrival)
{
    flight->schedule_arrival = strdup(flight_schedule_arrival);
}

void set_flight_real_departure(FLIGHT flight, char *flight_real_departure)
{
    flight->real_departure = strdup(flight_real_departure);
}

void set_flight_real_arrival(FLIGHT flight, char *flight_real_arrival)
{
    flight->real_arrival = strdup(flight_real_arrival);
}

void set_flight_pilot(FLIGHT flight, char *flight_pilot)
{
    flight->pilot = strdup(flight_pilot);
}

void set_flight_copilot(FLIGHT flight, char *flight_copilot)
{
    flight->copilot = strdup(flight_copilot);
}

void set_flight_notes(FLIGHT flight, char *flight_notes)
{
    flight->notes = strdup(flight_notes);
}

void set_flight_delay(FLIGHT flight, char *schedule_departure, char *real_departure)
{
    flight->delay = date_to_epoch(real_departure) - date_to_epoch(schedule_departure);
}

void add_flight_passenger(FLIGHT flight, char *user_id)
{
    g_hash_table_insert(flight->passengers, user_id, NULL);// adds passengers to the flight, using their user_id 
}

int verify_total_seats(char *total_seats)// if has letters doesnt pass, goes right to the erros file
{
    if (!total_seats)
        return 0;

    if (is_valid_int(total_seats))
        return 1;

    return 0;
}

long verify_schedule_dates_flight(char *schedule_departure_date, char *schedule_arrival_date)
{

    long schedule_departure_date_epoch = date_to_epoch(schedule_departure_date);
    long schedule_arrival_date_epoch = date_to_epoch(schedule_arrival_date);

    return schedule_departure_date_epoch - schedule_arrival_date_epoch;
}

long verify_real_dates_flight(char *real_departure_date, char *real_arrival_date)
{

    long real_departure_date_epoch = date_to_epoch(real_departure_date);
    long real_arrival_date_epoch = date_to_epoch(real_arrival_date);

    return real_departure_date_epoch - real_arrival_date_epoch;
}

int verify_airport(char *airport)
{
    if (!airport)
        return 0;

    to_lower(airport);

    return strlen(airport) == 3 && isalpha(airport[0]) && isalpha(airport[1]) && isalpha(airport[2]);
}

long verify_flights_dates(char *departure_date, char *arrival_date)
{
    return date_num_diff(departure_date, arrival_date) > 0;
}

int verify_flight(char **parameters)
{
    // printf("Flight %s\n", parameters[0]);
    // if (!(parameters[0] && strlen(parameters[0])))
    //     printf("ID wrong\n");
    // if (!(parameters[1] && strlen(parameters[1])))
    //     printf("Airline wrong\n");
    // if (!(parameters[2] && strlen(parameters[2])))
    //     printf("Plane model wrong\n");
    // if (!(verify_airport(parameters[4])))
    //     printf("Origin airport wrong\n");
    // if (!(verify_airport(parameters[5])))
    //     printf("Destination airport wrong\n");
    // if (!(verify_date_time_format(parameters[6])))
    //     printf("schedule departure wrong\n");
    // if (!(verify_date_time_format(parameters[7])))
    //     printf("schedule arrival wrong\n");
    // if (!(verify_flights_dates(parameters[6], parameters[7])))
    //     printf("schedule departure date is before than schedule arrival date wrong\n");
    // if (!(verify_date_time_format(parameters[8])))
    //     printf("real departure wrong\n");
    // if (!(verify_date_time_format(parameters[9])))
    //     printf("real arrival wrong\n");
    // if (!(verify_flights_dates(parameters[8], parameters[9])))
    //     printf("real departure date is before than real arrival date wrong\n");
    // if (!(parameters[10] && strlen(parameters[10])))
    //     printf("Pilot wrong\n");
    // if (!(parameters[11] && strlen(parameters[11])))
    //     printf("Copilot wrong\n");

    // Verifies if the id isn't NULL or empty
    if (parameters[0] && strlen(parameters[0]) &&
        // Verifies if the airline isn't NULL or empty
        parameters[1] && strlen(parameters[1]) &&
        // Verifies if the plane model isn't NULL or empty
        parameters[2] && strlen(parameters[2]) &&
        // Verifies if the total seats field is a valid number 
        verify_total_seats(parameters[3]) &&
        // Verifies if the origin and destination airports follow the format
        verify_airport(parameters[4]) && verify_airport(parameters[5]) &&
        // Verifies if the schedule departure follows the format
        verify_date_time_format(parameters[6]) &&
        // Verifies if the schedule arrival follows the format
        verify_date_time_format(parameters[7]) &&
        // Verifies if schedule departure date is before than schedule arrival date
        verify_flights_dates(parameters[6], parameters[7]) &&
        // Verifies if the real departure follows the format
        verify_date_time_format(parameters[8]) &&
        // Verifies if the real arrival follows the format
        verify_date_time_format(parameters[9]) &&
        // Verifies if real departure date is before than real arrival date
        verify_flights_dates(parameters[8], parameters[9]) &&
        // Verifies if the pilot isn't NULL or empty
        parameters[10] && strlen(parameters[10]) &&
        // Verifies if the copilot isn't NULL or empty
        parameters[11] && strlen(parameters[11]))
        return 1;

    return 0;
}

FLIGHT build_flight(char **flight_params)
{
    if (!verify_flight(flight_params))
        return NULL;

    // int passengers = count_flight_passengers(passengers_catalog, flight_params[0]);

    // // Verifies if the total seats aren't less then the passengers
    // if (!verify_total_seats(flight_params[3], passengers))
    //     return NULL;

    FLIGHT flight = create_flight();

    set_flight_id(flight, flight_params[0]);
    set_flight_airline(flight, flight_params[1]);
    set_flight_plane_model(flight, flight_params[2]);
    set_flight_total_seats(flight, flight_params[3]);
    set_flight_origin(flight, flight_params[4]);
    set_flight_destination(flight, flight_params[5]);
    set_flight_schedule_departure(flight, flight_params[6]);
    set_flight_schedule_arrival(flight, flight_params[7]);
    set_flight_real_departure(flight, flight_params[8]);
    set_flight_real_arrival(flight, flight_params[9]);
    set_flight_pilot(flight, flight_params[10]);
    set_flight_copilot(flight, flight_params[11]);
    set_flight_notes(flight, flight_params[12]);

    set_flight_delay(flight, flight_params[6], flight_params[8]);
    // flight->n_passengers = passengers;

    return flight;
}

void free_flight(FLIGHT flight)
{
    if (flight->id != NULL)
        free(flight->id);
    if (flight->airline != NULL)
        free(flight->airline);
    if (flight->plane_model != NULL)
        free(flight->plane_model);
    if (flight->origin != NULL)
        free(flight->origin);
    if (flight->destination != NULL)
        free(flight->destination);
    if (flight->schedule_departure != NULL)
        free(flight->schedule_departure);
    if (flight->schedule_arrival != NULL)
        free(flight->schedule_arrival);
    if (flight->real_departure != NULL)
        free(flight->real_departure);
    if (flight->real_arrival != NULL)
        free(flight->real_arrival);
    if (flight->pilot != NULL)
        free(flight->pilot);
    if (flight->copilot != NULL)
        free(flight->copilot);
    if (flight->notes != NULL)
        free(flight->notes);
    if (flight->passengers)
        g_hash_table_destroy(flight->passengers);

    free(flight);
}