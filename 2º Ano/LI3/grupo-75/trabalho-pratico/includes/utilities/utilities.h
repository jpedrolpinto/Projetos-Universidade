#ifndef UTILITIES_H
#define UTILITIES_H

#include "catalogs/catalogs.h"
#include "catalogs/users_catalog.h"
#include "catalogs/flights_catalog.h"
#include "catalogs/reservations_catalog.h"

typedef enum errors
{
  ERR_OPENING_USERS_FILE = 1,
  ERR_OPENING_FLIGHTS_FILE = 2,
  ERR_OPENING_PASSENGERS_FILE = 3,
  ERR_OPENING_RESERVATIONS_FILE = 4,
  ERR_OPENING_QUERIES_FILE = 5,
  ERR_CREATING_DIRECTORY = 6,
  ERR_OPENING_OUTPUT_FILE = 7,
} ERRORS;

int create_directory(char *folder);

int setup_catalogs(CATALOGS catalogs, char *folder);

void parse_commands(const char *input, char ***result, int *count);

int read_queries(char *queries_filename, CATALOGS catalogs);

char *get_error_as_string(ERRORS error);

#endif