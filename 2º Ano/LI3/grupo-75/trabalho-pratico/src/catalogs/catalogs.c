#include "catalogs/catalogs.h"

CATALOGS create_catalogs()
{
    CATALOGS catalogs;
    catalogs.users_catalog = create_users_catalog();
    catalogs.flights_catalog = create_flights_catalog();
    catalogs.reservations_catalog = create_reservations_catalog();

    return catalogs;
}

void free_catalogs(CATALOGS catalogs)
{
    free_users_catalog(catalogs.users_catalog);
    free_flights_catalog(catalogs.flights_catalog);
    free_reservations_catalog(catalogs.reservations_catalog);
}