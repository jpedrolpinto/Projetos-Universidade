#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "io/queries.h"
#include "utilities/date_time.h"
#include "utilities/utilities.h"
#include "utilities/data_handler.h"

QUERY1_RESULT query1(char **commands, CATALOGS catalogs)
{
    char *query_id = commands[0];
    char *hotel_id = commands[1];

    int formatted = !strcmp(query_id, "1F");

    QUERY1_RESULT result = NULL;

    USER user = get_user_by_id(catalogs.users_catalog, hotel_id);

    if (user && user->account_status == ACTIVE)
    {
        result = malloc(sizeof(struct query1_result));
        result->formatted = formatted;
        result->user = malloc(sizeof(struct query1_user));
        result->flight = NULL;
        result->reservation = NULL;

        result->user->name = strdup(user->name);
        result->user->sex = user->sex;
        result->user->age = user->age;
        result->user->country_code = strdup(user->country);
        result->user->passport = strdup(user->passport);
        result->user->number_of_flights = count_flights(user);

        int number_of_reservations = 0;
        double total_spent = 0;
        count_user_reservations_and_total_spent(catalogs.reservations_catalog, user->id, &number_of_reservations, &total_spent);
        result->user->number_of_reservations = number_of_reservations;
        result->user->total_spent = total_spent;

        return result;
    }
    else if (user)
        // Encontrou o user, mas está inativo
        return NULL;

    FLIGHT flight = get_flight_by_id(catalogs.flights_catalog, hotel_id);

    if (flight)
    {
        result = malloc(sizeof(struct query1_result));
        result->formatted = formatted;
        result->flight = malloc(sizeof(struct query1_flight));
        result->user = NULL;
        result->reservation = NULL;

        result->flight->airline = strdup(flight->airline);
        result->flight->plane_model = strdup(flight->plane_model);
        result->flight->origin = strdup(flight->origin);
        result->flight->destination = strdup(flight->destination);
        result->flight->schedule_departure_date = strdup(flight->schedule_departure);
        result->flight->schedule_arrival_date = strdup(flight->schedule_arrival);
        result->flight->passengers = count_passengers(flight);
        result->flight->delay = flight->delay;

        return result;
    }

    RESERVATION reservation = get_reservation_by_id(catalogs.reservations_catalog, hotel_id);

    if (reservation)
    {
        result = malloc(sizeof(struct query1_result));
        result->formatted = formatted;
        result->reservation = malloc(sizeof(struct query1_reservation));
        result->flight = NULL;
        result->user = NULL;

        result->reservation->hotel_id = strdup(reservation->hotel_id);
        result->reservation->hotel_name = strdup(reservation->hotel_name);
        result->reservation->hotel_stars = reservation->hotel_stars;
        result->reservation->begin_date = strdup(reservation->begin_date);
        result->reservation->end_date = strdup(reservation->end_date);
        result->reservation->includes_breakfast = reservation->includes_breakfast;
        result->reservation->nights = reservation->nights;
        result->reservation->total_price = reservation->total_price;

        return result;
    }
    return result;
}

void free_query1_user(QUERY1_USER query1_user)
{
    free(query1_user->name);
    free(query1_user->country_code);
    free(query1_user->passport);
    free(query1_user);
}

void free_query1_flight(QUERY1_FLIGHT query1_flight)
{
    free(query1_flight->airline);
    free(query1_flight->plane_model);
    free(query1_flight->origin);
    free(query1_flight->destination);
    free(query1_flight->schedule_departure_date);
    free(query1_flight->schedule_arrival_date);
    free(query1_flight);
}

void free_query1_reservation(QUERY1_RESERVATION query1_reservation)
{
    free(query1_reservation->hotel_id);
    free(query1_reservation->hotel_name);
    free(query1_reservation->begin_date);
    free(query1_reservation->end_date);
    free(query1_reservation);
}

void free_query1_result(QUERY1_RESULT query1_result)
{
    if (!query1_result)
        return;

    if (query1_result->user)
        free_query1_user(query1_result->user);

    if (query1_result->flight)
        free_query1_flight(query1_result->flight);

    if (query1_result->reservation)
        free_query1_reservation(query1_result->reservation);

    free(query1_result);
}

int sort_result_2(QUERY2_ENTITIES entity1, QUERY2_ENTITIES entity2)
{
    long date1 = date_to_epoch(entity1->date);
    long date2 = date_to_epoch(entity2->date);

    if (date1 == date2)
        return strcmp(entity1->id, entity2->id);
    else
        return date2 - date1;
}

QUERY2_RESULT query2(char **commands, CATALOGS catalogs)
{
    char *query_id = commands[0];
    char *hotel_id = commands[1];
    char *type = commands[2];

    int formatted = !strcmp(query_id, "2F");

    QUERY2_RESULT result = malloc(sizeof(struct query2_result));
    result->formatted = formatted;
    result->entities = NULL;

    USER user = get_user_by_id(catalogs.users_catalog, hotel_id);

    if (user && user->account_status == ACTIVE)
    {
        if (!type || !strcmp(type, "flights"))
        {
            GList *flights_it = user->flights;
            while (flights_it)
            {
                char *flight_id = (char *)flights_it->data;
                FLIGHT flight = get_flight_by_id(catalogs.flights_catalog, flight_id);

                QUERY2_ENTITIES entities_result = malloc(sizeof(struct query2_entities));
                entities_result->id = strdup(flight->id);
                entities_result->date = strdup(flight->schedule_departure);
                entities_result->date[10] = '\0';

                if (!type)
                    entities_result->type = "flight";
                else
                    entities_result->type = NULL;

                result->entities = g_list_append(result->entities, entities_result);

                flights_it = flights_it->next;
            }
        }

        if (!type || !strcmp(type, "reservations"))
        {
            GList *reservations_it = user->reservations;
            while (reservations_it)
            {
                char *reservation_id = (char *)reservations_it->data;
                RESERVATION reservation = get_reservation_by_id(catalogs.reservations_catalog, reservation_id);

                QUERY2_ENTITIES entities_result = malloc(sizeof(struct query2_entities));
                entities_result->id = strdup(reservation->id);
                entities_result->date = strdup(reservation->begin_date);

                if (!type)
                    entities_result->type = "reservation";
                else
                    entities_result->type = NULL;

                result->entities = g_list_append(result->entities, entities_result);

                reservations_it = reservations_it->next;
            }
        }

        if (result->entities)
            result->entities = g_list_sort(result->entities, (GCompareFunc)sort_result_2);
    }

    return result;
}

void free_query2_entities(QUERY2_ENTITIES query2_entities)
{
    if (query2_entities)
    {
        free(query2_entities->date);
        free(query2_entities->id);
        free(query2_entities);
    }
}

void free_query2_result(QUERY2_RESULT query2_result)
{
    if (!query2_result)
        return;

    if (query2_result->entities)
        g_list_free_full(g_steal_pointer(&query2_result->entities), (GDestroyNotify)free_query2_entities);

    free(query2_result);
}

QUERY3_RESULT query3(char **commands, RESERVATIONS_CATALOG reservations_catalog)
{
    char *query_id = commands[0];
    char *hotel_id = commands[1];

    int formatted = !strcmp(query_id, "3F");

    QUERY3_RESULT result = NULL;

    if (hotel_id == NULL)
        return result;

    double average_rating = hotel_average_rating(reservations_catalog, hotel_id);

    if (average_rating >= 0)
    {
        result = malloc(sizeof(struct query3_result));
        result->formatted = formatted;
        result->reservation = malloc(sizeof(struct query3_reservation));
        result->reservation->average_rating = average_rating;
    }

    return result;
}

void free_query3_result(QUERY3_RESULT query3_result)
{
    if (!query3_result)
        return;

    if (query3_result->reservation)
    {
        free(query3_result->reservation);
    }

    free(query3_result);
}

int sort_result_4(QUERY4_RESERVATION reservation1, QUERY4_RESERVATION reservation2) {
  long begin_date_1_epoch = date_to_epoch(reservation1->begin_date);
  long begin_date_2_epoch = date_to_epoch(reservation2->begin_date);

  if (begin_date_1_epoch == begin_date_2_epoch) {
    return strcmp(reservation1->id, reservation2->id);
  }
  else {
    return begin_date_2_epoch - begin_date_1_epoch;
  }
}

QUERY4_RESULT query4(char** commands, RESERVATIONS_CATALOG reservations_catalog ){
  char *query_id = commands[0];
  char *hotel_id = commands[1];

  int formatted = !strcmp(query_id, "4F");

  GList *reservations = g_hash_table_get_values(reservations_catalog->reservations);
  GList *aux = reservations;
  QUERY4_RESULT result = malloc(sizeof(struct query4_result));
  result->formatted = formatted;
  result->reservations = NULL;

  while(aux){

    RESERVATION reservation = (RESERVATION)aux->data;

    if (reservation) {

      if(!strcmp(reservation->hotel_id, hotel_id)){

        QUERY4_RESERVATION reservation_result = malloc(sizeof(struct query4_reservation));
        reservation_result->id = strdup(reservation->id);
        reservation_result->begin_date = strdup(reservation->begin_date);
        reservation_result->end_date = strdup(reservation->end_date);
        reservation_result->user_id = strdup(reservation->user_id);
        reservation_result->rating = strdup(reservation->rating);
        reservation_result->total_price = reservation->total_price;

        result->reservations = g_list_append(result->reservations, reservation_result);
      }
    }

    aux = aux->next;
  }

  result->reservations = g_list_sort(result->reservations, (GCompareFunc)sort_result_4);

  g_list_free(reservations);

return result;
}

void free_query4_reservation(QUERY4_RESERVATION reservation_result)
{
    if (reservation_result)
    {
        free(reservation_result->id);
        free(reservation_result->begin_date);
        free(reservation_result->end_date);
        free(reservation_result->user_id);
        free(reservation_result->rating);
        free(reservation_result);
    }
}

void free_query4_result(QUERY4_RESULT query4_result) {
  if (!query4_result) return;

  if (query4_result->reservations) {
    g_list_free_full(g_steal_pointer(&query4_result->reservations), (GDestroyNotify)free_query4_reservation);
  }

  free(query4_result);
}

int sort_result_5(QUERY5_FLIGHT flight1, QUERY5_FLIGHT flight2) {

  long date1 = date_to_epoch(flight1->schedule_departure_date);
  long date2 = date_to_epoch(flight2->schedule_departure_date);

  if (date1 == date2) {
    return strcmp(flight1->id, flight2->id);
  }
  else {
    return date2 - date1;
  }
}

QUERY5_RESULT query5(char **commands,FLIGHTS_CATALOG flights_catalog){
  char *query_id = commands[0];
  char *hotel_id = commands[1];
  long begin_date = date_to_epoch(commands[2]);
  long end_date = date_to_epoch(commands[3]);

  int formatted = !strcmp(query_id, "5F");

  GList *flights = g_hash_table_get_values(flights_catalog->flights);
  GList *aux = flights;
  QUERY5_RESULT result = malloc(sizeof(struct query5_result));
  result->formatted = formatted;
  result->flights = NULL;

  while(aux){

    FLIGHT flight = (FLIGHT)aux->data;

    if (flight) {

      if(!strcmp(flight->origin, hotel_id)){

        long departure = date_to_epoch(flight->real_departure);

        if(departure >= begin_date && departure <= end_date){

        QUERY5_FLIGHT flight_result = malloc(sizeof(struct query5_flight));
        flight_result->id = strdup(flight->id);
        flight_result->schedule_departure_date = strdup(flight->schedule_departure);
        flight_result->destination = strdup(flight->destination);
        flight_result->airline = strdup(flight->airline);
        flight_result->plane_model = strdup(flight->plane_model);

        result->flights = g_list_append(result->flights, flight_result);

        }
      }
    }

    aux = aux->next;

  }

  result->flights = g_list_sort(result->flights, (GCompareFunc)sort_result_5);

  g_list_free(flights);

  return result;
}

void free_query5_flight(QUERY5_FLIGHT flight_result)
{
    if (flight_result)
    {
        free(flight_result->id);
        free(flight_result->schedule_departure_date);
        free(flight_result->destination);
        free(flight_result->airline);
        free(flight_result->plane_model);
        free(flight_result);
    }
}

void free_query5_result(QUERY5_RESULT query5_result) {
  if (!query5_result) return;

  if (query5_result->flights) {
    g_list_free_full(g_steal_pointer(&query5_result->flights), (GDestroyNotify)free_query5_flight);
  }

  free(query5_result);
}

int sort_result_6(QUERY6_FLIGHT flight1, QUERY6_FLIGHT flight2){
  if (flight1->passengers == flight2->passengers) {
    return strcmp(flight1->airport, flight2->airport);
  }
  else {
    return flight2->passengers - flight1->passengers;
  }

}

int query6_flight_cmp(QUERY6_FLIGHT flight1, QUERY6_FLIGHT flight2) {
  return strcmp(flight1->airport, flight2->airport);
}

QUERY6_RESULT query6(char** commands, FLIGHTS_CATALOG flights_catalog){
  char *query_id = commands[0];
  int year = string_to_int(commands[1]);

  int formatted = !strcmp(query_id, "6F");

  GList *flights = g_hash_table_get_values(flights_catalog->flights);
  GList *aux = flights;
  QUERY6_RESULT result = malloc(sizeof(struct query6_result));
  result->formatted = formatted;
  result->flights = NULL;

  while(aux){

    FLIGHT flight = (FLIGHT)aux->data;

    if (flight) {
      to_upper(flight->origin);
      to_upper(flight->destination);

      int flight_year = get_year(flight->schedule_departure);

      if(flight_year == year){

        // ORIGIN
        QUERY6_FLIGHT flight_origin_result = malloc(sizeof(struct query6_flight));
        flight_origin_result->airport = strdup(flight->origin);
        flight_origin_result->passengers = count_passengers(flight);

        GList* flight_result_origin_ptr = g_list_find_custom(result->flights, flight_origin_result, (GCompareFunc)query6_flight_cmp);

        if (flight_result_origin_ptr) {
          // O aeroporto já existe na lista, então somar o nº de passageiros do voo atual
          ((QUERY6_FLIGHT)flight_result_origin_ptr->data)->passengers += count_passengers(flight);
          free(flight_origin_result->airport);
          free(flight_origin_result);
        }
        else {
          result->flights = g_list_append(result->flights, flight_origin_result);
        }

        // DESTINATION
        QUERY6_FLIGHT flight_destination_result = malloc(sizeof(struct query6_flight));
        flight_destination_result->airport = strdup(flight->destination);
        flight_destination_result->passengers = count_passengers(flight);

        GList* flight_destination_result_ptr = g_list_find_custom(result->flights, flight_destination_result, (GCompareFunc)query6_flight_cmp);

        if (flight_destination_result_ptr) {
          // O aeroporto já existe na lista, então somar o nº de passageiros do voo atual
          ((QUERY6_FLIGHT)flight_destination_result_ptr->data)->passengers += count_passengers(flight);
          free(flight_destination_result->airport);
          free(flight_destination_result);
        }
        else {
          result->flights = g_list_append(result->flights, flight_destination_result);
        }
      }
    }

    aux = aux->next;
  }

  result->flights = g_list_sort(result->flights, (GCompareFunc)sort_result_6);

  g_list_free(flights);

  return result;
}

void free_query6_flight(QUERY6_FLIGHT flight_result)
{
    if (flight_result)
    {
        free(flight_result->airport);
        free(flight_result);
    }
}

void free_query6_result(QUERY6_RESULT query6_result) {
  if (!query6_result) return;

  if (query6_result->flights) {
    g_list_free_full(g_steal_pointer(&query6_result->flights), (GDestroyNotify)free_query6_flight);
  }

  free(query6_result);
}

int sort_result_7(QUERY7_FLIGHT flight1, QUERY7_FLIGHT flight2) {

  if (flight1->median == flight2->median) {
    return strcmp(flight1->origin, flight2->origin);
  }
  else {
    return flight2->median - flight1->median;
  }
}

QUERY7_RESULT query7(char** commands, FLIGHTS_CATALOG flights_catalog){
  char *query_id = commands[0];

  int formatted = !strcmp(query_id, "7F");

  GList *flights = g_hash_table_get_values(flights_catalog->flights);
  GList *aux = flights;
  QUERY7_RESULT result = malloc(sizeof(struct query7_result));
  result->formatted = formatted;
  result->flights = NULL;

  GList *visited_airports = NULL;

  while(aux){

    FLIGHT flight = (FLIGHT)aux->data;

    // Vê se o aeroporto já está na lista, ou seja, se já foi calculada a mediana dos delays para este aeroporto
    // Se já estiver, não volta a calcular
    GList* airport = g_list_find_custom(visited_airports, flight->origin, (GCompareFunc)strcmp);

    if (flight && !airport) {

      QUERY7_FLIGHT flight_result = malloc(sizeof(struct query7_flight));

      flight_result->origin = strdup(flight->origin);
      flight_result->median = airport_delays(flights_catalog, flight->origin);

      result->flights = g_list_append(result->flights, flight_result);

      visited_airports = g_list_append(visited_airports, flight->origin);

    }

    aux = aux->next;
  }

  result->flights = g_list_sort(result->flights, (GCompareFunc)sort_result_7);

  g_list_free(flights);

  if (visited_airports) {
    g_list_free(visited_airports);
  }

  return result;
}

void free_query7_flight(QUERY7_FLIGHT flight_result)
{
    if (flight_result)
    {
        free(flight_result->origin);
        free(flight_result);
    }
}

void free_query7_result(QUERY7_RESULT query7_result) {
  if (!query7_result) return;

  if (query7_result->flights) {
    g_list_free_full(g_steal_pointer(&query7_result->flights), (GDestroyNotify)free_query7_flight);
  }

  free(query7_result);
}

QUERY8_RESULT query8(char** commands, RESERVATIONS_CATALOG reservations_catalog) {
  char *query_id = commands[0];
  char *hotel_id = commands[1];
  long begin_date = date_to_epoch(commands[2]);
  long end_date = date_to_epoch(commands[3]);

  int formatted = !strcmp(query_id, "8F");

  GList *reservations = g_hash_table_get_values(reservations_catalog->reservations);
  GList *aux = reservations;

  QUERY8_RESULT result = malloc(sizeof(struct query8_result));
  result->formatted = formatted;
  result->revenue = 0;

  while(aux){

    RESERVATION reservation = (RESERVATION)aux->data;

    if (reservation) {

      if(!strcmp(reservation->hotel_id, hotel_id)){

        long checkin = date_to_epoch(reservation->begin_date);
        long checkout = date_to_epoch(reservation->end_date);

        int num_days = 0;
        if(checkin >= begin_date && checkout <= end_date){
          num_days = time_diff_days(checkout, checkin);
        }
        else if (checkin < begin_date && checkout > begin_date && checkout < end_date) {
          num_days = time_diff_days(checkout, begin_date);
        }
        else if (checkin >= begin_date && checkin <= end_date && checkout > end_date) {
          num_days = time_diff_days(end_date, checkin) + 1;
        }
        else if (checkin < begin_date && checkout > end_date) {
          num_days = time_diff_days(end_date, begin_date) + 1;
        }

        result->revenue += (reservation->price_per_night * num_days);
      }
    }

    aux = aux->next;
  }

  g_list_free(reservations);

  return result;
}

void free_query8_result(QUERY8_RESULT query8_result) {
  if (!query8_result) return;

  free(query8_result);
}

void remove_at_index(char *str, int index) {
    for (int i = index; i < (int)strlen(str); i++) {
      str[i] = str[i + 1];
    }
}

int remove_first_accent(char *str) {

    char *accentedChars = "àáâãäåèéêëìíîïòóôõöùúûüýñç";
    char *unaccentedChars = "aaaaaeeeeiiiiooooouuuuync";

    int has_accents = 0;

    int i = 0;
    while ((*accentedChars) != 0)
    {
      gunichar c = g_utf8_get_char(accentedChars); // obtains characters from accentedChars
      gchar* acChar = g_utf8_strchr(str, strlen(str), c);// searchs for the occurrence of the chracter c in str
      if (acChar) { // if c was found in str then
        int index_to_replace = acChar - str; // position of the character

        str[index_to_replace] = unaccentedChars[i / 2];// replaces the character with an accent with the same character without an accent
        remove_at_index(str, index_to_replace + 1); // removes the next position(where the accent was)

        has_accents = 1;
      }
      ++accentedChars;
      i++;
    }

    return has_accents;
}

void remove_accents(char *str) {
  while (remove_first_accent(str)) {} // calls repeatedly the function remove_first_accent to remove the next accent if it exists
}

int remove_first_hyphen(char *str) {
  int has_hyphen = 0;

  gchar* hyphChar = g_utf8_strchr(str, strlen(str), '-');
  if (hyphChar) {
    int index_to_replace = hyphChar - str;

    remove_at_index(str, index_to_replace);

    has_hyphen = 1;
  }

  return has_hyphen;
}

void remove_hyphens(char *str) {
  while (remove_first_hyphen(str)) {}
}

int remove_first_space(char *str) {
  int has_space = 0;

  gchar* hyphChar = g_utf8_strchr(str, strlen(str), ' ');
  if (hyphChar) {
    int index_to_replace = hyphChar - str;

    remove_at_index(str, index_to_replace);

    has_space = 1;
  }

  return has_space;
}

void remove_spaces(char *str) {
  while (remove_first_space(str)) {}
}

int sort_result_9(QUERY9_USER user1, QUERY9_USER user2) {
  char* user1_name = g_utf8_strdown(user1->name, -1);
  char* user2_name = g_utf8_strdown(user2->name, -1);

  remove_accents(user1_name);
  remove_accents(user2_name);
  remove_hyphens(user1_name);
  remove_hyphens(user2_name);
  remove_spaces(user1_name);
  remove_spaces(user2_name);

  int res = g_utf8_collate(user1_name, user2_name);
  if (res == 0) {
    char* user1_id = g_utf8_strdown(user1->id, -1);
    char* user2_id = g_utf8_strdown(user2->id, -1);

    remove_accents(user1_id);
    remove_accents(user2_id);
    remove_hyphens(user1_id);
    remove_hyphens(user2_id);
    remove_spaces(user1_id);
    remove_spaces(user2_id);

    res = g_utf8_collate(user1_id, user2_id);

    free(user1_id);
    free(user2_id);
  }

  free(user1_name);
  free(user2_name);

  return res;
}

QUERY9_RESULT query9(char** commands, USERS_CATALOG users_catalog) {
  char *query_id = commands[0];
  char *hotel_id = commands[1];

  int formatted = !strcmp(query_id, "9F");

  GList *users = g_hash_table_get_values(users_catalog->users);
  GList *aux = users;
  QUERY9_RESULT result = malloc(sizeof(struct query9_result));
  result->formatted = formatted;
  result->users = NULL;

  while(aux){

    USER user = (USER)aux->data;

    if (user && tem_prefixo(hotel_id, user->name)) {

      if(user->account_status == ACTIVE){
        QUERY9_USER user_result = malloc(sizeof(struct query9_user));
        user_result->id = strdup(user->id);
        user_result->name = strdup(user->name);
        result->users = g_list_append(result->users, user_result);
      }
    }

    aux = aux->next;
  }

  result->users = g_list_sort(result->users, (GCompareFunc)sort_result_9);

  g_list_free(users);

  return result;
}

void free_query9_user(QUERY9_USER user_result)
{
    if (user_result)
    {
        free(user_result->id);
        free(user_result->name);
        free(user_result);
    }
}

void free_query9_result(QUERY9_RESULT query9_result) {
  if (!query9_result) return;

  if (query9_result->users) {
    g_list_free_full(g_steal_pointer(&query9_result->users), (GDestroyNotify)free_query9_user);
  }

  free(query9_result);
}

// void free_query10_metrics(QUERY10_METRICS metrics) {
//   free(metrics);
// }

// void free_query10_result(QUERY10_RESULT query10_result) {
//   if (!query10_result) return;

//   if (query10_result->metrics) {
//     g_hash_table_destroy(query10_result->metrics);
//   }

//   free(query10_result);
// }

// QUERY10_METRICS create_metrics(int period) {
//   QUERY10_METRICS metrics = malloc(sizeof(struct query10_metrics));
//   metrics->period = period;
//   metrics->flights = 0;
//   metrics->reservations = 0;
//   metrics->total_passengers = 0;
//   metrics->unique_passengers = 0;
//   metrics->users = 0;
//   return metrics;
// }

// QUERY10_RESULT query10(char** commands, RESERVATIONS_CATALOG reservations_catalog, FLIGHTS_CATALOG flights_catalog, USERS_CATALOG users_catalog, PASSENGERS_CATALOG passengers_catalog){
//   char *query_id = commands[0];
//   char *year_arg = commands[1];
//   char *month_arg = commands[2];

//   int formatted = !strcmp(query_id, "10F");

//   QUERY10_RESULT result = malloc(sizeof(struct query10_result));
//   result->formatted = formatted;
//   result->metrics = g_hash_table_new_full(g_direct_hash, g_direct_equal, NULL, (GDestroyNotify)free_query10_metrics);

//   // USERS
//   GList *users = g_hash_table_get_values(users_catalog->users);
//   GList *aux = users;

//   while(aux){
//     USER user = (USER)aux->data;

//     int period = 0;
//     if (!year_arg) {
//       period = get_year(user->account_creation);
//     }
//     else if (!month_arg) {
//       int year = string_to_int(year_arg);
//       int account_creation_year = get_year(user->account_creation);

//       if (account_creation_year == year) {
//         period = get_month(user->account_creation);
//       }
//     }
//     else {
//       int year = string_to_int(year_arg);
//       int month = string_to_int(month_arg);
//       int account_creation_year = get_year(user->account_creation);
//       int account_creation_month = get_month(user->account_creation);

//       if (account_creation_year == year && account_creation_month == month) {
//         period = get_day(user->account_creation);
//       }
//     }

//     if (period > 0) {
//       QUERY10_METRICS metrics = (QUERY10_METRICS)g_hash_table_lookup(result->metrics, GINT_TO_POINTER(period));

//       if (metrics) {
//         metrics->users++;
//       }
//       else {
//         metrics = create_metrics(period);
//         metrics->users = 1;
//         g_hash_table_insert(result->metrics, GINT_TO_POINTER(period), metrics);
//       }
//     }

//     aux = aux->next;
//   }
//   g_list_free(users);

//   return result;
// }
