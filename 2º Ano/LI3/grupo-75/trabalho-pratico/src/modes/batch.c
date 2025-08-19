#include <stdio.h>
#include <glib.h>

#include "modes/batch.h"
#include "catalogs/catalogs.h"
#include "utilities/utilities.h"

int batch(char **argv)
{
    // Create output directory
    if (create_directory("Resultados") != 0)
    {
        return ERR_CREATING_DIRECTORY;
    }

    // Create catalogs
    CATALOGS catalogs = create_catalogs();

    int res = setup_catalogs(catalogs, argv[1]);
    if (res != 0)
    {
        return res;
    }
    printf("Catalogs complete.\n");
    // exit(0);

    res = read_queries(argv[2], catalogs);

    // printf("n_users = %d\n", g_hash_table_size(users_catalog->users));

    // printf("n_reservations = %d\n", g_hash_table_size(reservations_catalog->reservations));

    // printf("n_passengers = %d\n", g_hash_table_size(passengers_catalog->passengers));

    // printf("n_flights = %d\n", g_hash_table_size(flights_catalog->flights));

    free_catalogs(catalogs);

    return 0;
}