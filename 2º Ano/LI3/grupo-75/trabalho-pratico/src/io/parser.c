#include "io/parser.h"
#include "utilities/utilities.h"
#include "types/reservation.h"
#include "types/passenger.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char **parse_line(char *line, char *separator, int max_tokens)
{
    int token_count = 0;
    char **tokens = malloc(sizeof(char *) * max_tokens); // max_tokens - parameters(one token - one parameter)
    char *temp = strstr(line, separator);                // finds the first occurrence of the separator
    char *token = NULL;

    while (temp != NULL)
    {                                       // if separator is found do...
        token = temp != line ? line : NULL; // if temp is the begining of line then token is NULL otherwise token is the part before the separator
        tokens[token_count++] = token;      // saves the token in the array
        line = temp + 1;
        *temp = '\0';                   // puts a null character in the separator place
        temp = strstr(line, separator); // finds the next separator
    }

    token = line;
    // saves the final part of the whole line
    tokens[token_count++] = token;

    return tokens;
}

void parse_users_file(FILE *file, CATALOGS catalogs)
{
    char *line = NULL;
    size_t alloc_size = 0;

    FILE *users_errors_file = fopen("Resultados/users_errors.csv", "w");

    // Verifica se o arquivo foi aberto com sucesso
    if (users_errors_file == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo users_errors.csv\n");
        return;
    }

    // Skip first line
    int res = getline(&line, &alloc_size, file);
    if (res <= 0)
        return;
    line[strlen(line) - 1] = '\0';
    fprintf(users_errors_file, "%s\n", line); // adds the head line to the errors file

    while (getline(&line, &alloc_size, file) != -1)
    {
        // Remove the \n from the end of the line
        line[strlen(line) - 1] = '\0';

        char *saved_line = strdup(line);
        char **tokens = parse_line(line, SEPARATOR, MAX_USER_TOKENS);

        USER user = build_user(tokens);
        if (user)
            insert_user(catalogs.users_catalog, user);
        else
            fprintf(users_errors_file, "%s\n", saved_line);

        free(saved_line);
        free(tokens);
    }

    printf("Users: %d\n", g_hash_table_size(catalogs.users_catalog->users));
    free(line);
    fclose(users_errors_file);
}

void parse_reservations_file(FILE *file, CATALOGS catalogs)
{
    char *line = NULL;
    size_t alloc_size = 0;

    FILE *reservations_errors_file = fopen("Resultados/reservations_errors.csv", "w");

    // Verifica se o arquivo foi aberto com sucesso
    if (reservations_errors_file == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo reservations_errors.csv\n");
        return;
    }

    // Skip first line
    int res = getline(&line, &alloc_size, file);
    if (res <= 0)
        return;
    line[strlen(line) - 1] = '\0';
    fprintf(reservations_errors_file, "%s\n", line); // adds the head line to the errors file

    while (getline(&line, &alloc_size, file) != -1)
    {
        // Remove the \n from the end of the line
        line[strlen(line) - 1] = '\0';

        char *saved_line = strdup(line);
        char **tokens = parse_line(line, SEPARATOR, MAX_RESERVATION_TOKENS);

        USER user = get_user_by_id(catalogs.users_catalog, tokens[1]);
        RESERVATION reservation = NULL;
        if (user)
            reservation = build_reservation(tokens);
        if (reservation)
        {
            insert_reservation(catalogs.reservations_catalog, reservation);
            USER user = get_user_by_id(catalogs.users_catalog, reservation->user_id);
            add_user_reservation(user, reservation->id, catalogs.reservations_catalog);
        }
        else
            fprintf(reservations_errors_file, "%s\n", saved_line);

        free(saved_line);
        free(tokens);
    }

    printf("Reservations: %d\n", g_hash_table_size(catalogs.reservations_catalog->reservations));
    free(line);
    fclose(reservations_errors_file);
}

void parse_flights_file(FILE *file, CATALOGS catalogs)
{
    char *line = NULL;
    size_t alloc_size = 0;

    FILE *flights_errors_file = fopen("Resultados/flights_errors.csv", "w");

    // Verifica se o arquivo foi aberto com sucesso
    if (flights_errors_file == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo flights_errors.csv\n");
        return;
    }

    // Skip first line
    int res = getline(&line, &alloc_size, file);
    if (res <= 0)
        return;
    line[strlen(line) - 1] = '\0';
    fprintf(flights_errors_file, "%s\n", line); // adds the head line to the errors file

    while (getline(&line, &alloc_size, file) != -1)
    {
        // Remove the \n from the end of the line
        line[strlen(line) - 1] = '\0';

        char *saved_line = strdup(line);
        char **tokens = parse_line(line, SEPARATOR, MAX_FLIGHT_TOKENS);

        FLIGHT flight = build_flight(tokens);
        if (flight)
            insert_flight(catalogs.flights_catalog, flight);
        else
            fprintf(flights_errors_file, "%s\n", saved_line);

        free(saved_line);
        free(tokens);
    }

    printf("Flights: %d\n", g_hash_table_size(catalogs.flights_catalog->flights));
    free(line);
    fclose(flights_errors_file);
}

void invalidate_overfull_flights(USERS_CATALOG users_catalog, FLIGHTS_CATALOG flights_catalog, FILE *passengers_errors_file)
{

    FILE *flights_errors_file = fopen("Resultados/flights_errors.csv", "a");

    if (flights_errors_file == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo flights_errors.csv\n");
        return;
    }

    GHashTableIter iter;
    gpointer key, value;

    g_hash_table_iter_init(&iter, flights_catalog->flights);
    while (g_hash_table_iter_next(&iter, &key, &value))
    {
        FLIGHT f = (FLIGHT)value;
        if (f->total_seats < count_passengers(f))
        {
            fprintf(flights_errors_file, "%s;%s;%s;%d;%s;%s;%s;%s;%s;%s;%s;%s;%s\n",
                    f->id, f->airline, f->plane_model, f->total_seats, f->origin,
                    f->destination, f->schedule_departure, f->schedule_arrival,
                    f->real_departure, f->real_arrival, f->pilot, f->copilot, f->notes);

            GHashTableIter passenger_iter;
            gpointer passenger_id, passenger_v;
            g_hash_table_iter_init(&passenger_iter, f->passengers);
            while (g_hash_table_iter_next(&passenger_iter, &passenger_id, &passenger_v))
            {
                fprintf(passengers_errors_file, "%s;%s\n", f->id, (char *)passenger_id);
                // Remove flight from the user's list
                USER u = get_user_by_id(users_catalog, passenger_id);
                remove_flight(u, f->id);
            }

            g_hash_table_iter_remove(&iter);
        }
    }

    fclose(flights_errors_file);
}

void parse_passengers_file(FILE *file, CATALOGS catalogs)
{
    char *line = NULL;
    size_t alloc_size = 0;

    FILE *passengers_errors_file = fopen("Resultados/passengers_errors.csv", "w");

    // Verifica se o arquivo foi aberto com sucesso
    if (passengers_errors_file == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo passengers_errors.csv\n");
        return;
    }

    // Skip first line
    int res = getline(&line, &alloc_size, file);
    if (res <= 0)
        return;
    line[strlen(line) - 1] = '\0';
    fprintf(passengers_errors_file, "%s\n", line); // adds the head line to the errors file

    int i = 1;

    while (getline(&line, &alloc_size, file) != -1)
    {
        // Remove the \n from the end of the line
        line[strlen(line) - 1] = '\0';

        // if (i % 500000 == 0)
        //     printf("line %d\n", i);

        char *saved_line = strdup(line);
        char **tokens = parse_line(line, SEPARATOR, MAX_FLIGHT_TOKENS);

        int passenger_res = process_passenger(tokens, catalogs.users_catalog, catalogs.flights_catalog);
        if (!passenger_res)
            fprintf(passengers_errors_file, "%s\n", saved_line);

        i++;

        free(saved_line);
        free(tokens);
    }

    invalidate_overfull_flights(catalogs.users_catalog, catalogs.flights_catalog, passengers_errors_file);

    free(line);
    fclose(passengers_errors_file);
}