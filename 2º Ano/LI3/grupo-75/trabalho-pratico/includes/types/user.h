#ifndef USER_H
#define USER_H

#include <glib.h>
#include "catalogs/reservations_catalog.h"
#include "catalogs/flights_catalog.h"

typedef enum gender
{
    M,
    F
} gender;

typedef enum account_status
{
    ACTIVE,
    INACTIVE
} account_status;

typedef struct user
{
    char *id;
    char *name;
    char *email;
    char *phone;
    char *birthdate;
    gender sex;
    char *passport;
    char *country;
    char *address;
    char *account_creation;
    char *pay_method;
    account_status account_status;

    int age;
    GList *reservations;
    GList *flights;
} *USER;

// Funções
void add_user_reservation(USER user, char *reservation_id, RESERVATIONS_CATALOG reservations_catalog);

void add_user_flight(USER user, char *flight_id, FLIGHTS_CATALOG flights_catalog);

void remove_flight(USER user, char *flight_id);

int count_flights(USER user);

int verify_user(char **parameters);

USER build_user(char **user_params);

void free_user(USER user);

#endif