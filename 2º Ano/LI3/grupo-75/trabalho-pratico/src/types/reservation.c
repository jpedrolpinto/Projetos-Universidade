#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types/reservation.h"
#include "types/user.h"
#include "utilities/data_handler.h"
#include "utilities/date_time.h"

RESERVATION create_reservation()
{

    RESERVATION new_reservation = malloc(sizeof(struct reservation));
    new_reservation->id = NULL;
    new_reservation->user_id = NULL;
    new_reservation->hotel_id = NULL;
    new_reservation->hotel_name = NULL;
    new_reservation->address = NULL;
    new_reservation->begin_date = NULL;
    new_reservation->end_date = NULL;
    new_reservation->room_details = NULL;
    new_reservation->rating = NULL;
    new_reservation->comment = NULL;

    return new_reservation;
}

void set_reservation_id(RESERVATION reservation, char *reservation_id)
{
    reservation->id = strdup(reservation_id);
}

void set_reservation_user_id(RESERVATION reservation, char *reservation_user_id)
{
    reservation->user_id = strdup(reservation_user_id);
}

void set_reservation_hotel_id(RESERVATION reservation, char *reservation_hotel_id)
{
    reservation->hotel_id = strdup(reservation_hotel_id);
}

void set_reservation_hotel_name(RESERVATION reservation, char *reservation_hotel_name)
{
    reservation->hotel_name = strdup(reservation_hotel_name);
}

void set_reservation_hotel_stars(RESERVATION reservation, char *reservation_hotel_stars)
{
    reservation->hotel_stars = string_to_int(reservation_hotel_stars);
}

void set_reservation_city_tax(RESERVATION reservation, char *reservation_city_tax)
{
    reservation->city_tax = string_to_int(reservation_city_tax);
}

void set_reservation_address(RESERVATION reservation, char *reservation_address)
{
    reservation->address = strdup(reservation_address);
}

void set_reservation_begin_date(RESERVATION reservation, char *reservation_begin_date)
{
    reservation->begin_date = strdup(reservation_begin_date);
}

void set_reservation_end_date(RESERVATION reservation, char *reservation_end_date)
{
    reservation->end_date = strdup(reservation_end_date);
}

void set_reservation_price_per_night(RESERVATION reservation, char *reservation_price_per_night)
{
    reservation->price_per_night = string_to_int(reservation_price_per_night);
}

void set_reservation_includes_breakfast(RESERVATION reservation, char *reservation_includes_breakfast)
{
    if (reservation_includes_breakfast && (!strcmp(reservation_includes_breakfast, "true") || !strcmp(reservation_includes_breakfast, "t") || !strcmp(reservation_includes_breakfast, "1")))
    {
        reservation->includes_breakfast = TRUE;
    }
    else
    {
        reservation->includes_breakfast = FALSE;
    }
}

void set_reservation_room_details(RESERVATION reservation, char *reservation_room_details)
{
    reservation->room_details = strdup(reservation_room_details);
}

void set_reservation_rating(RESERVATION reservation, char *reservation_rating)
{
    reservation->rating = strdup(reservation_rating);
}

void set_reservation_comment(RESERVATION reservation, char *reservation_comment)
{
    reservation->comment = strdup(reservation_comment);
}

void reservation_nights(RESERVATION reservation, char *begin_date, char *end_date)
{

    reservation->nights = (date_to_epoch(end_date) - date_to_epoch(begin_date)) / 86400; // retorna o numero de noites
}

void total_price(RESERVATION reservation)
{
    double price_per_night = (double)reservation->price_per_night;
    double city_tax = (double)reservation->city_tax;
    double n_nights = (double)reservation->nights;

    reservation->total_price = (price_per_night * n_nights) + (((price_per_night * n_nights) / 100) * city_tax);
}

int verify_hotel_stars(char *hotel_stars)
{
    if (is_valid_int(hotel_stars))
    {
        int stars = string_to_int(hotel_stars);
        if (stars >= 1 && stars <= 5)
            return 1;
    }

    return 0;
}

int verify_city_tax(char *city_tax)
{
    if (is_valid_int(city_tax))
    {
        if (string_to_int(city_tax) >= 0)
            return 1; // Número inteiro não-negativo
    }

    return 0;
}

int verify_price_per_night(char *price_per_night)
{
    if (is_valid_int(price_per_night))
    {
        if ((string_to_int(price_per_night) > 0))
            return 1;
    }

    return 0;
}

int verify_includes_breakfast(char *includes_breakfast)
{
    if (!includes_breakfast)
        return 1;

    to_lower(includes_breakfast);

    if (!strcmp("true", includes_breakfast) || !strcmp("t", includes_breakfast) || !strcmp("1", includes_breakfast))
        return 1;
    if (!strcmp("false", includes_breakfast) || !strcmp("f", includes_breakfast) || !strcmp("0", includes_breakfast) || !strcmp("", includes_breakfast))
        return 1;

    return 0;
}

int verify_rating(char *rating)
{
    if (rating)
    {
        if (!strlen(rating)) // empty string
            return 1;
        int rat = string_to_int(rating);
        if (rat >= 1 && rat <= 5)
            return 1;
    }

    return 0;
}

long verify_reservation_dates(char *begin_date, char *end_date)
{
    return date_num_diff(begin_date, end_date) > 0;
}

int verify_reservation(char **parameters)
{
    // Verifies if the id isn't NULL or empty
    if (parameters[0] && strlen(parameters[0]) &&
        // Verifies if the user id isn't NULL or empty
        parameters[1] && strlen(parameters[1]) &&
        // Verifies if the hotel id isn't NULL or empty
        parameters[2] && strlen(parameters[2]) &&
        // Verifies if the hotel name isn't NULL or empty
        parameters[3] && strlen(parameters[3]) &&
        // Verifies if the hotel stars follows the format
        verify_hotel_stars(parameters[4]) &&
        // Verifies if the city tax follows the format
        verify_city_tax(parameters[5]) &&
        // Verifies if the address isn't NULL or empty
        parameters[6] && strlen(parameters[6]) &&
        // Verifies if the begin date follows the format
        verify_date_format(parameters[7]) &&
        // Verifies if the end date follows the format
        verify_date_format(parameters[8]) &&
        // Verifies if begin date is before than end date
        verify_reservation_dates(parameters[7], parameters[8]) &&
        // Verifies if the price per night follows the format
        verify_price_per_night(parameters[9]) &&
        // Verifies if includes_breakfast follows the format
        verify_includes_breakfast(parameters[10]) &&
        // Verifies if the rating follows the format
        verify_rating(parameters[12]))
        // If all fields are valid then the user is valid
        return 1;

    return 0;
}

RESERVATION build_reservation(char **reservation_params)
{
    if (!verify_reservation(reservation_params))
        return NULL;

    RESERVATION reservation = create_reservation();

    set_reservation_id(reservation, reservation_params[0]);
    set_reservation_user_id(reservation, reservation_params[1]);
    set_reservation_hotel_id(reservation, reservation_params[2]);
    set_reservation_hotel_name(reservation, reservation_params[3]);
    set_reservation_hotel_stars(reservation, reservation_params[4]);
    set_reservation_city_tax(reservation, reservation_params[5]);
    set_reservation_address(reservation, reservation_params[6]);
    set_reservation_begin_date(reservation, reservation_params[7]);
    set_reservation_end_date(reservation, reservation_params[8]);
    set_reservation_price_per_night(reservation, reservation_params[9]);
    set_reservation_includes_breakfast(reservation, reservation_params[10]);
    set_reservation_room_details(reservation, reservation_params[11]);
    set_reservation_rating(reservation, reservation_params[12]);
    set_reservation_comment(reservation, reservation_params[13]);

    reservation_nights(reservation, reservation_params[7], reservation_params[8]);
    total_price(reservation);

    return reservation;
}

void free_reservation(RESERVATION reservation)
{
    if (reservation->id != NULL)
        free(reservation->id);
    if (reservation->user_id != NULL)
        free(reservation->user_id);
    if (reservation->hotel_id != NULL)
        free(reservation->hotel_id);
    if (reservation->hotel_name != NULL)
        free(reservation->hotel_name);
    if (reservation->address != NULL)
        free(reservation->address);
    if (reservation->begin_date != NULL)
        free(reservation->begin_date);
    if (reservation->end_date != NULL)
        free(reservation->end_date);
    if (reservation->room_details != NULL)
        free(reservation->room_details);
    if (reservation->rating != NULL)
        free(reservation->rating);
    if (reservation->comment != NULL)
        free(reservation->comment);

    free(reservation);
}