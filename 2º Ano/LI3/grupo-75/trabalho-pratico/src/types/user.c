#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types/user.h"
#include "utilities/data_handler.h"
#include "utilities/date_time.h"

USER create_user()
{
    USER new_user = malloc(sizeof(struct user));
    new_user->id = NULL;
    new_user->name = NULL;
    new_user->email = NULL;
    new_user->phone = NULL;
    new_user->passport = NULL;
    new_user->country = NULL;
    new_user->address = NULL;
    new_user->pay_method = NULL;

    new_user->reservations = NULL; // reservations of each user
    new_user->flights = NULL; // flihgts of each user
    // GLists

    return new_user;
}

void remove_flight(USER user, char *flight_id)
{
    user->flights = g_list_remove(user->flights, flight_id);
}

int count_flights(USER user)
{
    return g_list_length(user->flights);
}

void set_user_id(USER user, char *user_id)
{
    user->id = strdup(user_id); // strdup so any alterations dont affect the original string
}

void set_user_name(USER user, char *user_name)
{
    user->name = strdup(user_name);
}

void set_user_email(USER user, char *user_email)
{
    user->email = strdup(user_email);
}

void set_user_phone(USER user, char *user_phone)
{
    user->phone = strdup(user_phone);
}

void set_user_birthdate(USER user, char *user_birthdate)
{
    user->birthdate = strdup(user_birthdate);
}

void set_user_sex(USER user, char *user_sex)
{
    gender sex;

    if (!strcmp(user_sex, "m"))
    {
        sex = M;
    }
    else
    {
        sex = F;
    }
    user->sex = sex;
}

void set_user_passport(USER user, char *user_passport)
{
    user->passport = strdup(user_passport);
}

void set_user_country(USER user, char *user_country)
{
    user->country = strdup(user_country);
}

void set_user_address(USER user, char *user_address)
{
    user->address = strdup(user_address);
}

void set_user_account_creation(USER user, char *user_account_creation)
{
    user->account_creation = strdup(user_account_creation);
}

void set_user_pay_method(USER user, char *user_pay_method)
{
    user->pay_method = strdup(user_pay_method);
}

void set_user_account_status(USER user, char *user_account_status)
{
    account_status account_status;

    if (!strcmp(user_account_status, "active"))
    {
        account_status = ACTIVE;
    }
    else
    {
        account_status = INACTIVE;
    }
    user->account_status = account_status;
}

void set_user_age(USER user, char *birth_date)
{
    user->age = (date_to_epoch(CURRENT_DATE) - date_to_epoch(birth_date)) / 31556926; // returns age in years
}

gint user_reservation_ordering(gconstpointer reservation_id_a, gconstpointer reservation_id_b, gpointer reservations_catalog)
{
    RESERVATION reservation_a = g_hash_table_lookup(((RESERVATIONS_CATALOG)reservations_catalog)->reservations, reservation_id_a);
    RESERVATION reservation_b = g_hash_table_lookup(((RESERVATIONS_CATALOG)reservations_catalog)->reservations, reservation_id_b);
    int res = date_num_diff(reservation_a->begin_date, reservation_b->begin_date);
    if (!res)
        res = strcmp(reservation_id_a, reservation_id_b);
    return res;
}

void add_user_reservation(USER user, char *reservation_id, RESERVATIONS_CATALOG reservations_catalog)
{
    if (user->reservations)
        user->reservations = g_list_insert_sorted_with_data(user->reservations, reservation_id, user_reservation_ordering, reservations_catalog);
    else
        user->reservations = g_list_prepend(NULL, reservation_id);
}

gint user_flight_ordering(gconstpointer flight_id_a, gconstpointer flight_id_b, gpointer flights_catalog)
{
    FLIGHT flight_a = g_hash_table_lookup(((FLIGHTS_CATALOG)flights_catalog)->flights, flight_id_a);
    FLIGHT flight_b = g_hash_table_lookup(((FLIGHTS_CATALOG)flights_catalog)->flights, flight_id_b);
    int res = date_num_diff(flight_a->schedule_departure, flight_b->schedule_departure);
    if (!res)
        res = strcmp(flight_id_a, flight_id_b);
    return res;
}

void add_user_flight(USER user, char *flight_id, FLIGHTS_CATALOG flights_catalog)
{
    if (user->flights)
        user->flights = g_list_insert_sorted_with_data(user->flights, flight_id, user_flight_ordering, flights_catalog);
    else
        user->flights = g_list_prepend(NULL, flight_id);
}

int verify_sex(char *user_sex)
{
    if (!user_sex)
        return 0;

    to_lower(user_sex);

    if (!strcmp("m", user_sex))
        return 1; // strcmp - if the strings are equal it returns 0(!0 -> 1), so in this case returns 1(valid)
    if (!strcmp("f", user_sex))
        return 1;

    return 0;
}

int verify_account_status(char *account_status)
{
    if (!account_status)
        return 0;

    to_lower(account_status);

    if (!strcmp("active", account_status))
        return 1;
    if (!strcmp("inactive", account_status))
        return 1;

    return 0;
}

int verify_country_code(char *country_code)
{

    return country_code && strlen(country_code) == 2 && isalpha(country_code[0]) && isalpha(country_code[1]);
}

int verify_email(char *email)
{
    char *at_ptr = strrchr(email, '@'); // finds the last ocurrence of @ in email(pointer)
    char *dot_ptr = strrchr(email, '.');

    if (at_ptr && dot_ptr)
    {
        int at_pos = at_ptr - email;
        int dot_pos = dot_ptr - email;

        return at_pos > 0 && dot_pos > at_pos + 1 && dot_pos < (int)strlen(email) - 2;
    }

    return 0;
}

long verify_user_dates(char *birth_date, char *account_creation)
{
    return date_num_diff(birth_date, account_creation) > 0;
}

int verify_user(char **parameters)
{
    // if (!(parameters[0] && strlen(parameters[0]))) {
    //     printf("User %s\n", parameters[0]);
    //     printf("ID wrong\n");
    //     return 0;
    // }
    // if (!(parameters[1] && strlen(parameters[1]))) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Name wrong\n");
    //     return 0;
    // }
    // if (!verify_email(parameters[2])) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Email wrong\n");
    //     return 0;
    // }
    // if (!(parameters[3] && strlen(parameters[3]))) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Phone number wrong\n");
    //     return 0;
    // }
    // if (!verify_date_format(parameters[4])) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Birth date wrong\n");
    //     return 0;
    // }
    // if (!verify_sex(parameters[5])) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Sex wrong\n");
    //     return 0;
    // }
    // if (!(parameters[6] && strlen(parameters[6]))) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Passport wrong\n");
    //     return 0;
    // }
    // if (!verify_country_code(parameters[7])) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Country wrong\n");
    //     return 0;
    // }
    // if (!(parameters[8] && strlen(parameters[8]))) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Address wrong\n");
    //     return 0;
    // }
    // if (!verify_date_time_format(parameters[9])) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Account creation date wrong\n");
    //     return 0;
    // }
    // if (!verify_user_dates(parameters[4], parameters[9])) {
    //     printf("User %s\n", parameters[0]);
    //     printf("User dates wrong\n");
    //     return 0;
    // }
    // if (!(parameters[10] && strlen(parameters[10]))) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Pay method wrong\n");
    //     return 0;
    // }
    // if (!verify_account_status(parameters[11])) {
    //     printf("User %s\n", parameters[0]);
    //     printf("Account status wrong\n");
    //     return 0;
    // }

    // Verifies if the id isn't NULL or empty
    if (parameters[0] && strlen(parameters[0]) &&
        // Verifies if the name isn't NULL or empty
        parameters[1] && strlen(parameters[1]) &&
        // Verifies if the email follows the format
        verify_email(parameters[2]) &&
        // Verifies if the phone number isn't NULL or empty
        parameters[3] && strlen(parameters[3]) &&
        // Verifies if the birth date follows the format
        verify_date_format(parameters[4]) &&
        // Verifies if the sex follows the format
        verify_sex(parameters[5]) &&
        // Verifies if the passport isn't NULL or empty
        parameters[6] && strlen(parameters[6]) &&
        // Verifies if country code follows the format
        verify_country_code(parameters[7]) &&
        // Verifies if the address isn't NULL or empty
        parameters[8] && strlen(parameters[8]) &&
        // Verifies if the account creation follows the format
        verify_date_time_format(parameters[9]) &&
        // Verifies if birthdate is before the account creation date
        verify_user_dates(parameters[4], parameters[9]) &&
        // Verifies if the pay method isn't NULL or empty
        parameters[10] && strlen(parameters[10]) &&
        // Verifies if account status follows the format
        verify_account_status(parameters[11]))
        // If all fields are valid then the user is valid
        return 1;

    return 0;
}

USER build_user(char **user_params)
{
    if (!verify_user(user_params))
        return NULL;

    USER user = create_user();

    set_user_id(user, user_params[0]);
    set_user_name(user, user_params[1]);
    set_user_email(user, user_params[2]);
    set_user_phone(user, user_params[3]);
    set_user_birthdate(user, user_params[4]);
    set_user_age(user, user_params[4]);
    set_user_sex(user, user_params[5]);
    set_user_passport(user, user_params[6]);
    set_user_country(user, user_params[7]);
    set_user_address(user, user_params[8]);
    set_user_account_creation(user, user_params[9]);
    set_user_pay_method(user, user_params[10]);
    set_user_account_status(user, user_params[11]);

    return user;
}

void free_user(USER user)
{
    if (user->id != NULL)
        free(user->id);
    if (user->name != NULL)
        free(user->name);
    if (user->email != NULL)
        free(user->email);
    if (user->phone != NULL)
        free(user->phone);
    if (user->birthdate != NULL)
        free(user->birthdate);
    if (user->passport != NULL)
        free(user->passport);
    if (user->country != NULL)
        free(user->country);
    if (user->address != NULL)
        free(user->address);
    if (user->account_creation != NULL)
        free(user->account_creation);
    if (user->pay_method != NULL)
        free(user->pay_method);
    if (user->reservations != NULL)
        g_list_free(user->reservations);
    if (user->flights != NULL)
        g_list_free(user->flights);
    free(user);
}