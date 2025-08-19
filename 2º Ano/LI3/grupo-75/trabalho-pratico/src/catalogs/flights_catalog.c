#include <stdio.h>
#include "catalogs/flights_catalog.h"
#include "utilities/data_handler.h"

FLIGHTS_CATALOG create_flights_catalog(void)
{
	FLIGHTS_CATALOG new_catalog = malloc(sizeof(struct flights_catalog));

	new_catalog->flights = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)free_flight);

	return new_catalog;
}

void insert_flight(FLIGHTS_CATALOG catalog, FLIGHT flight)
{
	g_hash_table_insert(catalog->flights, flight->id, flight); // hashtable, key, valores
}

FLIGHT get_flight_by_id(FLIGHTS_CATALOG catalog, char *flight_id)
{
	if (catalog == NULL || catalog->flights == NULL || flight_id == NULL)
		// Tratar casos de catálogo, tabela de hash ou ID de usuário nulos.
		return NULL;

	FLIGHT flight = (FLIGHT)g_hash_table_lookup(catalog->flights, (gconstpointer)flight_id); // retorna um voo, hashtable, key

	return flight;
}

double airport_delays(FLIGHTS_CATALOG catalog, char *origin)
{

	double median;

	GList *flights = g_hash_table_get_values(catalog->flights);
	GList *aux = flights;

	int delays[1000000];
	int counter = 0;

	while (aux)
	{
		FLIGHT flight = (FLIGHT)aux->data;

		if (!strcmp(flight->origin, origin))
		{
			delays[counter] = flight->delay;
			counter++;
		}

		aux = aux->next;
	}

	g_list_free(flights);

	selection_sort(delays, counter); // organiza os delays de forma crescente para fazer mediana

	// Calcula a mediana
	if (counter % 2 == 0)
		median = (delays[counter / 2 - 1] + delays[counter / 2]) / 2; // se delays for par
	else
		median = delays[counter / 2]; // se delays impar

	return median;
}

void free_flights_catalog(FLIGHTS_CATALOG catalog)
{
	g_hash_table_destroy(catalog->flights);
	free(catalog);
}