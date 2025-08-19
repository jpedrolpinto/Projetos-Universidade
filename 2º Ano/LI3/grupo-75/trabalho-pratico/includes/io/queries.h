#ifndef QUERIER_H
#define QUERIER_H

#include <glib.h>

#include "catalogs/catalogs.h"
#include "catalogs/users_catalog.h"
#include "catalogs/reservations_catalog.h"
#include "catalogs/flights_catalog.h"

typedef struct query1_user
{
    char *name;
    gender sex;
    int age;
    char *country_code;
    char *passport;
    int number_of_flights;
    int number_of_reservations;
    double total_spent;
    
} *QUERY1_USER;

typedef struct query1_flight
{
    char *airline;
    char *plane_model;
    char *origin;
    char *destination;
    char *schedule_departure_date;
    char *schedule_arrival_date;
    int passengers;
    int delay;
    
} *QUERY1_FLIGHT;

typedef struct query1_reservation
{
    char *hotel_id;
    char *hotel_name;
    int hotel_stars;
    char *begin_date;
    char *end_date;
    gboolean includes_breakfast;
    int nights;
    double total_price;
    
} *QUERY1_RESERVATION;

typedef struct query1_result
{
    QUERY1_USER user;
    QUERY1_FLIGHT flight;
    QUERY1_RESERVATION reservation;
    int formatted;
    
} *QUERY1_RESULT;

typedef struct query2_entities
{
    char *id;
    char *date;
    char *type;

} *QUERY2_ENTITIES;

typedef struct query2_result
{
    GList *entities;
    int formatted;
    
} *QUERY2_RESULT;

typedef struct query3_reservation
{
    double average_rating;
    
} *QUERY3_RESERVATION;

typedef struct query3_result
{
    QUERY3_RESERVATION reservation;
    int formatted;
    
} *QUERY3_RESULT;

typedef struct query4_reservation
{
    char *id;
    char *begin_date;
    char *end_date;
    char *user_id;
    char *rating;
    double total_price;
    
} *QUERY4_RESERVATION;

typedef struct query4_result
{
    GList *reservations;
    int formatted;
    
} *QUERY4_RESULT;

typedef struct query5_flight
{
    char* id;
    char* schedule_departure_date;
    char* destination;
    char* airline;
    char* plane_model;
    
} *QUERY5_FLIGHT;

typedef struct query5_result
{
    GList *flights;
    int formatted;
    
} *QUERY5_RESULT;

typedef struct query6_flight
{
    char *airport;
    int passengers;
    
} *QUERY6_FLIGHT;

typedef struct query6_result
{
    GList *flights;
    int formatted;
    
} *QUERY6_RESULT;


typedef struct query7_flight
{
    char *origin;
    double median;
    
} *QUERY7_FLIGHT;

typedef struct query7_result
{
    GList *flights;
    int formatted;
    
} *QUERY7_RESULT;

typedef struct query8_result
{
    int revenue;
    int formatted;
    
} *QUERY8_RESULT;

typedef struct query9_user
{
    char* id;
    char *name;
    
} *QUERY9_USER;

typedef struct query9_result
{
    GList *users;
    int formatted;
    
} *QUERY9_RESULT;

typedef struct query10_metrics 
{
    int period;
    int users;
    int flights;
    int total_passengers;
    int unique_passengers;
    int reservations;

} *QUERY10_METRICS;

typedef struct query10_result
{
    GHashTable *metrics;
    int formatted;
    
} *QUERY10_RESULT;



QUERY1_RESULT query1(char **commands, CATALOGS catalogs);
void free_query1_result(QUERY1_RESULT query1_result);

QUERY2_RESULT query2(char** commands, CATALOGS catalogs);
void free_query2_result(QUERY2_RESULT query2_result);

QUERY3_RESULT query3(char **commands, RESERVATIONS_CATALOG reservations_catalog);
void free_query3_result(QUERY3_RESULT query3_result);

QUERY4_RESULT query4(char** commands, RESERVATIONS_CATALOG catalog);
void free_query4_result(QUERY4_RESULT query4_result);

QUERY5_RESULT query5(char** commands, FLIGHTS_CATALOG flights_catalog);
void free_query5_result(QUERY5_RESULT query5_result);

QUERY6_RESULT query6(char** commands, FLIGHTS_CATALOG flights_catalog);
void free_query6_result(QUERY6_RESULT query6_result);

QUERY7_RESULT query7(char** commands, FLIGHTS_CATALOG flights_catalog);
void free_query7_result(QUERY7_RESULT query7_result);

QUERY8_RESULT query8(char** commands, RESERVATIONS_CATALOG reservations_catalog);
void free_query8_result(QUERY8_RESULT query8_result);

QUERY9_RESULT query9(char** commands, USERS_CATALOG users_catalog);
void free_query9_result(QUERY9_RESULT query9_result);

// QUERY10_RESULT query10(char** commands, RESERVATIONS_CATALOG reservations_catalog, FLIGHTS_CATALOG flights_catalog, USERS_CATALOG users_catalog, PASSENGERS_CATALOG passengers_catalog);
// void free_query10_result(QUERY10_RESULT query10_result);


#endif