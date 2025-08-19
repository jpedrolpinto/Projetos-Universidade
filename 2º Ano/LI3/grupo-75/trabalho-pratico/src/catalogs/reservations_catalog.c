#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "catalogs/reservations_catalog.h"
#include "types/reservation.h"
#include "utilities/data_handler.h"

RESERVATIONS_CATALOG create_reservations_catalog(void)
{
    RESERVATIONS_CATALOG new_catalog = malloc(sizeof(struct reservations_catalog));

    new_catalog->reservations = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)free_reservation);

    return new_catalog;
}

void insert_reservation(RESERVATIONS_CATALOG catalog, RESERVATION reservation)
{
    g_hash_table_insert(catalog->reservations, reservation->id, reservation);
}

RESERVATION get_reservation_by_id(RESERVATIONS_CATALOG catalog, char *reservation_id)
{
    if (catalog == NULL || catalog->reservations == NULL || reservation_id == NULL)
    {
        // Tratar casos de catálogo, tabela de hash ou ID de usuário nulos.
        return NULL;
    }

    RESERVATION reservation = (RESERVATION)g_hash_table_lookup(catalog->reservations, (gconstpointer)reservation_id);
    if (reservation == NULL)
    {
        // O usuário não foi encontrado. Trate conforme necessário.
    }
    return reservation;
}

void count_user_reservations_and_total_spent(RESERVATIONS_CATALOG catalog, char *user_id, int *count, double *total_spent)
{
    int counter = 0;
    double total_spent_counter = 0;

    GList *reservations = g_hash_table_get_values(catalog->reservations);
    GList *aux = reservations;

    while (aux)
    {
        RESERVATION reservation = (RESERVATION)aux->data;

        if (!strcmp(reservation->user_id, user_id))
        {
            counter++;
            total_spent_counter += reservation->total_price;
        }

        aux = aux->next;
    }

    g_list_free(reservations);

    *count = counter;
    *total_spent = total_spent_counter;
}

double hotel_average_rating(RESERVATIONS_CATALOG catalog, char *hotel_id)
{
    double av_rating = 0;
    double counter = 0;

    GList *reservations = g_hash_table_get_values(catalog->reservations);
    GList *aux = reservations;

    while (aux)
    {
        RESERVATION reservation = (RESERVATION)aux->data; // pointer to struct reservation

        if (!strcmp(reservation->hotel_id, hotel_id))
        {
            if (is_valid_int(reservation->rating)) {
                counter++;
                av_rating += (double)string_to_int(reservation->rating);
            }
        }

        aux = aux->next; // moves to the next node, crucial to avoid an infinite loop
    }

    g_list_free(reservations);

    if (counter == 0)
        return -1;

    return av_rating / counter;
}

void free_reservations_catalog(RESERVATIONS_CATALOG catalog)
{
    g_hash_table_destroy(catalog->reservations);
    free(catalog);
}
