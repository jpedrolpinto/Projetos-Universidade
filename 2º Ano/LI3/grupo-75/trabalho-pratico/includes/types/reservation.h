#ifndef RESERVATION_H
#define RESERVATION_H

#include <glib.h>

typedef struct reservation
{
    char *id;
    char *user_id;
    char *hotel_id;
    char *hotel_name;
    int hotel_stars;
    int city_tax;
    char *address;
    char *begin_date;
    char *end_date;
    int price_per_night;
    gboolean includes_breakfast;
    char *room_details;
    char *rating;
    char *comment;

    int nights;
    double total_price;

} *RESERVATION;

RESERVATION build_reservation(char **reservation_params);

void free_reservation(RESERVATION reservation);

#endif