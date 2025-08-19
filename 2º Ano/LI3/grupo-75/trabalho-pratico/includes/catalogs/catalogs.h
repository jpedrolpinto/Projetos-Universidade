#ifndef CATALOGS_H
#define CATALOGS_H

#include <glib.h>

#include "catalogs/users_catalog.h"
#include "catalogs/flights_catalog.h"
#include "catalogs/reservations_catalog.h"

// Struct com todos os catálogos da aplicação.
// Campos:
// - 'users' é o catálogo de utilizadores;
// - 'flights' é o catálogo de voos;
// - 'reservations' é o catálogo de reservas.
typedef struct catalogs {
    USERS_CATALOG users_catalog;
    FLIGHTS_CATALOG flights_catalog;
    RESERVATIONS_CATALOG reservations_catalog;
} CATALOGS;

CATALOGS create_catalogs(void);

void free_catalogs(CATALOGS catalogs);

#endif