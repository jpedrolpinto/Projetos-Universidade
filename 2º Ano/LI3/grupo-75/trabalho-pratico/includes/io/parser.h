#ifndef PARSER_H
#define PARSER_H

#include "catalogs/catalogs.h"
// #include "catalogs/users_catalog.h"
// #include "catalogs/reservations_catalog.h"
// #include "catalogs/flights_catalog.h"

#include <glib.h>
#include <stdio.h>

// Parâmetros de cada estrutura
#define MAX_USER_TOKENS 12
#define MAX_FLIGHT_TOKENS 13
#define MAX_PASSENGER_TOKENS 2
#define MAX_RESERVATION_TOKENS 14

#define SEPARATOR ";"

char **parse_line(char *line, char *separator, int max_tokens);

void parse_users_file(FILE *file, CATALOGS catalogs);

void parse_reservations_file(FILE *file, CATALOGS catalogs);

void parse_flights_file(FILE *file, CATALOGS catalogs);

void parse_passengers_file(FILE *file, CATALOGS catalogs);

#endif