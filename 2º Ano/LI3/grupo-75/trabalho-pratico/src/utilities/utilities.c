#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "utilities/utilities.h"
#include "utilities/data_handler.h"
#include "types/flight.h"
#include "types/passenger.h"
#include "types/reservation.h"
#include "types/user.h"
#include "io/parser.h"
#include "io/queries.h"
#include "catalogs/catalogs.h"

// malloc is used to assign a specified amount of memory for an array to be created
char *create_filepath(char *folder, const char *filename)
{
    char *filepath = malloc(sizeof(char) * (strlen(folder) + strlen(filename) + 1));
    strcpy(filepath, folder);
    strcat(filepath, filename);

    return filepath;
}

FILE *open_dataset_file(char *folder, char *filename)
{
    char *filepath = create_filepath(folder, filename);
    FILE *file = fopen(filepath, "r");
    free(filepath);

    return file;
}

void write_errors(char *file, GList *errors)
{
    char *filename = malloc(sizeof(char) * 256);
    sprintf(filename, "Resultados/%s", file);

    FILE *output_file = fopen(filename, "w");

    // Verifica se o arquivo foi aberto com sucesso
    if (output_file == NULL)
    {
        fprintf(stderr, "Erro ao abrir o arquivo %s\n", filename);
        free(filename); // Libera a memória mesmo se o arquivo não foi aberto com sucesso
        return;
    }

    GList *aux = errors;

    while (aux)
    {
        char *line = (char *)aux->data;

        fprintf(output_file, "%s\n", line);
        aux = aux->next;
    }

    fclose(output_file);

    // Libera a memória depois de fechar o arquivo
    free(filename);
}

int setup_catalogs(CATALOGS catalogs, char *folder)
{
    FILE *users_file = open_dataset_file(folder, "/users.csv");
    if (users_file == NULL)
        return ERR_OPENING_USERS_FILE;

    FILE *reservations_file = open_dataset_file(folder, "/reservations.csv");
    if (reservations_file == NULL)
        return ERR_OPENING_RESERVATIONS_FILE;

    FILE *passengers_file = open_dataset_file(folder, "/passengers.csv");
    if (passengers_file == NULL)
        return ERR_OPENING_PASSENGERS_FILE;

    FILE *flights_file = open_dataset_file(folder, "/flights.csv");
    if (flights_file == NULL)
        return ERR_OPENING_FLIGHTS_FILE;

    parse_users_file(users_file, catalogs);
    printf("Users parsed.\n");
    parse_reservations_file(reservations_file, catalogs);
    printf("Reservations parsed.\n");
    parse_flights_file(flights_file, catalogs);
    printf("Flights parsed.\n");
    parse_passengers_file(passengers_file, catalogs);
    printf("Passengers parsed.\n");

    USER u = g_hash_table_lookup(catalogs.users_catalog->users, "RenPinho");
    GList *ufl = u->flights;
    while (ufl)
    {
        FLIGHT f = g_hash_table_lookup(catalogs.flights_catalog->flights, ufl->data);
        printf("Flight %s: %s\n", f->id, f->schedule_departure);
        ufl = ufl->next;
    }
    GList *rfl = u->reservations;
    while (rfl)
    {
        RESERVATION r = g_hash_table_lookup(catalogs.reservations_catalog->reservations, rfl->data);
        printf("Reservation %s: %s\n", r->id, r->begin_date);
        rfl = rfl->next;
    }

    fclose(users_file);
    fclose(reservations_file);
    fclose(passengers_file);
    fclose(flights_file);

    return 0;
}

int create_directory(char *folder)
{
    int ret = g_mkdir_with_parents(folder, 0777);

    if (ret == -1)
    {
        return ERR_CREATING_DIRECTORY;
    }

    return 0;
}

FILE *create_output_file(int queries_counter)
{
    char *filename = malloc(sizeof(char) * 256);
    sprintf(filename, "Resultados/command%d_output.txt", queries_counter);

    FILE *output_file = fopen(filename, "w");
    free(filename);

    return output_file;
}

char *gender_to_string(gender sex)
{
    switch (sex)
    {
    case M:
        return "M";
    case F:
        return "F";
    default:
        return "Unknown";
    }
}

char *includes_breakfast_to_string(gboolean includes_breakfast)
{
    switch (includes_breakfast)
    {
    case TRUE:
        return "True";
    case FALSE:
        return "False";
    default:
        return "Unknown";
    }
}

void write_query1_result(QUERY1_RESULT result, FILE *output_file)
{
    if (!result)
    {
        return;
    }

    if (result->user)
    {
        char *sex = strdup(gender_to_string(result->user->sex));

        if (!result->formatted)
            fprintf(output_file, "%s;%s;%d;%s;%s;%d;%d;%.3f\n",
                    result->user->name, sex, result->user->age, result->user->country_code, result->user->passport,
                    result->user->number_of_flights, result->user->number_of_reservations, result->user->total_spent);
        else
        {
            fprintf(output_file, "--- 1 ---\n"
                                 "name: %s\n"
                                 "sex: %s\n"
                                 "age: %d\n"
                                 "country_code: %s\n"
                                 "passport: %s\n"
                                 "number_of_flights: %d\n"
                                 "number_of_reservations: %d\n"
                                 "total_spent: %.3f\n",
                    result->user->name, sex, result->user->age, result->user->country_code, result->user->passport,
                    result->user->number_of_flights, result->user->number_of_reservations, result->user->total_spent);
        }

        free(sex);
    }
    else if (result->reservation)
    {
        char *breakfast = strdup(includes_breakfast_to_string(result->reservation->includes_breakfast));

        if (!result->formatted)
        {
            fprintf(output_file, "%s;%s;%d;%s;%s;%s;%d;%.3f\n", result->reservation->hotel_id, result->reservation->hotel_name, result->reservation->hotel_stars, result->reservation->begin_date, result->reservation->end_date, breakfast, result->reservation->nights, result->reservation->total_price);
        }
        else
        {
            fprintf(output_file, "--- 1 ---\n"
                                 "hotel_id: %s\n"
                                 "hotel_name: %s\n"
                                 "hotel_stars: %d\n"
                                 "begin_date: %s\n"
                                 "end_date: %s\n"
                                 "includes_breakfast: %s\n"
                                 "nights: %d\n"
                                 "total_price: %.3f\n",
                    result->reservation->hotel_id, result->reservation->hotel_name, result->reservation->hotel_stars, result->reservation->begin_date, result->reservation->end_date, breakfast, result->reservation->nights, result->reservation->total_price);
        }

        free(breakfast);
    }
    else if (result->flight)
    {

        if (!result->formatted)
        {
            fprintf(output_file, "%s;%s;%s;%s;%s;%s;%d;%d\n", result->flight->airline, result->flight->plane_model, result->flight->origin, result->flight->destination, result->flight->schedule_departure_date, result->flight->schedule_arrival_date, result->flight->passengers, result->flight->delay);
        }
        else
        {

            fprintf(output_file, "--- 1 ---\n"
                                 "airline: %s\n"
                                 "plane_model: %s\n"
                                 "origin: %s\n"
                                 "destination: %s\n"
                                 "schedule_departure_date: %s\n"
                                 "schedule_arrival_date: %s\n"
                                 "passengers: %d\n"
                                 "delay: %d\n",
                    result->flight->airline, result->flight->plane_model, result->flight->origin, result->flight->destination, result->flight->schedule_departure_date, result->flight->schedule_arrival_date, result->flight->passengers, result->flight->delay);
        }
    }
}

void write_query2_result(QUERY2_RESULT result, FILE *output_file)
{

    if (!result)
    {
        return;
    }

    GList *aux = result->entities;
    int i = 1;

    while (aux)
    {
        QUERY2_ENTITIES entities = (QUERY2_ENTITIES)aux->data;

        if (!result->formatted)
        {
            fprintf(output_file, "%s;%s%s%s\n", entities->id, entities->date, (entities->type ? ";" : ""), (entities->type ? entities->type : ""));
        }
        else
        {

            if (i > 1)
            {
                fprintf(output_file, "\n");
            }

            fprintf(output_file, "--- %d ---\n"
                                 "id: %s\n"
                                 "date: %s\n"
                                 "%s%s%s",
                    i, entities->id, entities->date, (entities->type ? "type: " : ""), (entities->type ? entities->type : ""), (entities->type ? "\n" : ""));
        }

        i++;
        aux = aux->next;
    }
}

void write_query3_result(QUERY3_RESULT result, FILE *output_file)
{

    if (!result)
    {
        return;
    }

    if (!result->formatted)
    {
        fprintf(output_file, "%.3f\n", result->reservation->average_rating);
    }
    else
    {

        fprintf(output_file, "--- 1 ---\n"
                             "rating: %.3f\n",
                result->reservation->average_rating);
    }
}

void write_query4_result(QUERY4_RESULT result, FILE *output_file)
{

    GList *aux = result->reservations;
    int i = 1;

    while (aux)
    {
        QUERY4_RESERVATION reservation = (QUERY4_RESERVATION)aux->data;

        if (!result)
        {
            return;
        }

        if (!result->formatted)
        {
            fprintf(output_file, "%s;%s;%s;%s;%s;%.3f\n", reservation->id, reservation->begin_date, reservation->end_date, reservation->user_id, reservation->rating, reservation->total_price);
        }
        else
        {

            if (i > 1)
            {
                fprintf(output_file, "\n");
            }

            fprintf(output_file, "--- %d ---\n"
                                 "id: %s\n"
                                 "begin_date: %s\n"
                                 "end_date: %s\n"
                                 "user_id: %s\n"
                                 "rating: %s\n"
                                 "total_price: %.3f\n",
                    i, reservation->id, reservation->begin_date, reservation->end_date, reservation->user_id, reservation->rating, reservation->total_price);
        }

        i++;
        aux = aux->next;
    }
}

void write_query5_result(QUERY5_RESULT result, FILE *output_file)
{

    GList *aux = result->flights;
    int i = 1;

    while (aux)
    {
        QUERY5_FLIGHT flight = (QUERY5_FLIGHT)aux->data;

        if (!result)
        {
            return;
        }

        if (!result->formatted)
        {
            fprintf(output_file, "%s;%s;%s;%s;%s\n", flight->id, flight->schedule_departure_date, flight->destination, flight->airline, flight->plane_model);
        }
        else
        {

            if (i > 1)
            {
                fprintf(output_file, "\n");
            }

            fprintf(output_file, "--- %d ---\n"
                                 "id: %s\n"
                                 "schedule_departure_date: %s\n"
                                 "destination: %s\n"
                                 "airline: %s\n"
                                 "plane_model: %s\n",
                    i, flight->id, flight->schedule_departure_date, flight->destination, flight->airline, flight->plane_model);
        }

        i++;
        aux = aux->next;
    }
}

void write_query6_result(QUERY6_RESULT result, FILE *output_file, int n_airports)
{
    if (!result)
    {
        return;
    }

    GList *aux = result->flights;
    int i = 1;

    while (aux && i <= n_airports)
    {
        QUERY6_FLIGHT flight = (QUERY6_FLIGHT)aux->data;

        if (!result->formatted)
        {
            fprintf(output_file, "%s;%d\n", flight->airport, flight->passengers);
        }
        else
        {

            if (i > 1)
            {
                fprintf(output_file, "\n");
            }

            fprintf(output_file, "--- %d ---\n"
                                 "name: %s\n"
                                 "passengers: %d\n",
                    i, flight->airport, flight->passengers);
        }

        i++;
        aux = aux->next;
    }
}

void write_query7_result(QUERY7_RESULT result, FILE *output_file, int n_airports)
{
    if (!result)
    {
        return;
    }

    GList *aux = result->flights;
    int i = 1;

    while (aux && i <= n_airports)
    {
        QUERY7_FLIGHT flight = (QUERY7_FLIGHT)aux->data;

        if (!result->formatted)
        {
            fprintf(output_file, "%s;%.0f\n", flight->origin, flight->median);
        }
        else
        {

            if (i > 1)
            {
                fprintf(output_file, "\n");
            }

            fprintf(output_file, "--- %d ---\n"
                                 "name: %s\n"
                                 "median: %.0f\n",
                    i, flight->origin, flight->median);
        }

        i++;
        aux = aux->next;
    }
}

void write_query8_result(QUERY8_RESULT result, FILE *output_file)
{
    if (!result)
    {
        return;
    }

    if (!result->formatted)
    {
        fprintf(output_file, "%d\n", result->revenue);
    }
    else
    {

        fprintf(output_file, "--- 1 ---\n"
                             "revenue: %d\n",
                result->revenue);
    }
}

void write_query9_result(QUERY9_RESULT result, FILE *output_file)
{
    if (!result)
    {
        return;
    }

    GList *aux = result->users;
    int i = 1;

    while (aux)
    {
        QUERY9_USER user = (QUERY9_USER)aux->data;

        if (!result->formatted)
        {
            fprintf(output_file, "%s;%s\n", user->id, user->name);
        }
        else
        {

            if (i > 1)
            {
                fprintf(output_file, "\n");
            }

            fprintf(output_file, "--- %d ---\n"
                                 "id: %s\n"
                                 "name: %s\n",
                    i, user->id, user->name);
        }

        i++;
        aux = aux->next;
    }
}

int sort_result_10(QUERY10_METRICS metrics1, QUERY10_METRICS metrics2)
{
    return metrics1->period - metrics2->period;
}

void write_query10_result(QUERY10_RESULT result, FILE *output_file, char **commands)
{
    if (!result)
    {
        return;
    }

    char *year_arg = commands[1];
    char *month_arg = commands[2];

    GList *metrics = g_hash_table_get_values(result->metrics);
    GList *aux = metrics;

    aux = g_list_sort(aux, (GCompareFunc)sort_result_10);

    int i = 1;

    while (aux)
    {
        QUERY10_METRICS metrics = (QUERY10_METRICS)aux->data;

        if (!result->formatted)
        {
            fprintf(output_file, "%d;%d;%d;%d;%d;%d\n", metrics->period, metrics->users, metrics->flights, metrics->total_passengers, metrics->unique_passengers, metrics->reservations);
        }
        else
        {

            if (i > 1)
            {
                fprintf(output_file, "\n");
            }

            fprintf(output_file, "--- %d ---\n"
                                 "%s: %d\n"
                                 "users: %d\n",
                    i, (!year_arg ? "year" : (!month_arg ? "month" : "day")), metrics->period, metrics->users);
        }

        i++;
        aux = aux->next;
    }
}

// Function to split a string by spaces, ignoring spaces within double quotes("")
void parse_commands(const char *input, char ***result, int *count)// 9 "Mateus Sim"
{
    *count = 0;// Count of the parsed tokens

    // Duplicate the input string to avoid modifying the original
    char *copy = strdup(input);

    // Initialize variables for tokenization
    char *token;
    char *rest = copy; //

    // Tokenize the string by spaces
    while ((token = strtok_r(rest, " ", &rest))) // 9 & "Mateus & Sim"
    {
        // Check if the token starts double quotes
        if (token[0] == '\"') // so only "Mateus enters the cycle
        {
            // Duplicate tokne to preserve the original
            char *init_token = strdup(token);
            int init_tok_len = strlen(init_token);
            char *quoted_tok = NULL;

            // Chechs if the token ends with double quote
            if (token[strlen(token) - 1] == '\"') 
            {
                quoted_tok = strdup(init_token);
            }
            else // "Mateus
            {
                while (token[strlen(token) - 1] != '\"')
                {
                    token = strtok_r(NULL, " ", &rest); // continues to tokenize the string and extracts Sim" 
                    if (token != NULL)
                    {
                        int seq_tok_len = strlen(token);// length of current token( Sim")
                        if (quoted_tok)
                            quoted_tok = realloc(quoted_tok, sizeof(char) * init_tok_len + seq_tok_len + 2);// resizes the memory block
                        else
                            quoted_tok = malloc(sizeof(char) * init_tok_len + seq_tok_len + 2);// allocates the memory for the first time
                        strcpy(quoted_tok, init_token); // copies init_token to quoted_tok( "Mateu )
                        strcat(quoted_tok, " "); // concatenate "Mateu with " " ("Mateu )
                        strcat(quoted_tok, token); // concatenate "Mateu  with Sim"
                    }
                    else
                    {
                        // Handle the case where a closing double quote is missing
                        fprintf(stderr, "Error: Missing closing double quote.\n");
                        free(init_token);
                        if (quoted_tok)
                            free(quoted_tok);
                        free(copy);
                        return;
                    }
                }
            }
            // Remove the double quotes from the concatenated token
            memmove(quoted_tok, quoted_tok + 1, strlen(quoted_tok));// quoted_tok + 1 -> quoted_tok()
            quoted_tok[strlen(quoted_tok) - 1] = '\0';
            // Store the concatenated token in the result array
            (*result)[(*count)++] = quoted_tok;

            free(init_token);
        }
        else // 9
        {
            // Store the token in the result array
            (*result)[(*count)++] = strdup(token); 
        }
    }

    // Free the copy of the input string
    free(copy);
}

int read_queries(char *queries_filename, CATALOGS catalogs)
{
    FILE *queries_file = fopen(queries_filename, "r");
    if (queries_file == NULL)
    {
        return ERR_OPENING_QUERIES_FILE;
    }

    char *line = NULL;
    // size_t so it can store the maxium size of the array
    size_t line_size = 0;
    int queries_counter = 1;

    // Format of the line: <query-id>[format-flag] [arg1...argN]
    while (getline(&line, &line_size, queries_file) != -1)
    {
        // Remove \n from the end of line
        line[strlen(line) - 1] = '\0';

        char **tokens = malloc(sizeof(char *) * 4);
        for (int t = 0; t < 4; t++)
            tokens[t] = NULL;

        int count_tokens;
        parse_commands(line, &tokens, &count_tokens);

        char *query_id = tokens[0];

        FILE *output_file = create_output_file(queries_counter);

        if (output_file == NULL)
        {
            return ERR_OPENING_OUTPUT_FILE;
        }
        if (!strcmp(query_id, "1") || !strcmp(query_id, "1F"))
        {
            QUERY1_RESULT result = query1(tokens, catalogs);
            write_query1_result(result, output_file);
            free_query1_result(result);
        }
        else if (!strcmp(query_id, "2") || !strcmp(query_id, "2F"))
        {
            QUERY2_RESULT result = query2(tokens, catalogs);
            write_query2_result(result, output_file);
            free_query2_result(result);
        }
        else if (!strcmp(query_id, "3") || !strcmp(query_id, "3F"))
        {
            QUERY3_RESULT result = query3(tokens, catalogs.reservations_catalog);
            write_query3_result(result, output_file);
            free_query3_result(result);
        }
        else if (!strcmp(query_id, "4") || !strcmp(query_id, "4F"))
        {
            QUERY4_RESULT result = query4(tokens, catalogs.reservations_catalog);
            write_query4_result(result, output_file);
            free_query4_result(result);
        }
        else if (!strcmp(query_id, "5") || !strcmp(query_id, "5F"))
        {
            QUERY5_RESULT result = query5(tokens, catalogs.flights_catalog);
            write_query5_result(result, output_file);
            free_query5_result(result);
        }
        else if (!strcmp(query_id, "6") || !strcmp(query_id, "6F"))
        {
            QUERY6_RESULT result = query6(tokens, catalogs.flights_catalog);
            int n_airports = string_to_int(tokens[2]);
            write_query6_result(result, output_file, n_airports);
            free_query6_result(result);
        }
        else if (!strcmp(query_id, "7") || !strcmp(query_id, "7F"))
        {
            QUERY7_RESULT result = query7(tokens, catalogs.flights_catalog);
            int n_airports = string_to_int(tokens[1]);
            write_query7_result(result, output_file, n_airports);
            free_query7_result(result);
        }
        else if (!strcmp(query_id, "8") || !strcmp(query_id, "8F"))
        {
            QUERY8_RESULT result = query8(tokens, catalogs.reservations_catalog);
            write_query8_result(result, output_file);
            free_query8_result(result);
        }
        else if (!strcmp(query_id, "9") || !strcmp(query_id, "9F"))
        {
            QUERY9_RESULT result = query9(tokens, catalogs.users_catalog);
            write_query9_result(result, output_file);
            free_query9_result(result);
        }
        // else if (!strcmp(query_id, "10") || !strcmp(query_id, "10F"))
        // {
        //     QUERY10_RESULT result = query10(tokens, reservations_catalog, flights_catalog, users_catalog, passengers_catalog);
        //     write_query10_result(result, output_file, tokens);
        //     free_query10_result(result);
        // }

        for (int t = 0; t < 4; t++)
            if (tokens[t])
                free(tokens[t]);
        free(tokens);
        fclose(output_file);

        queries_counter++;
    }

    free(line);
    fclose(queries_file);

    return 0;
}

char *get_error_as_string(ERRORS error)
{
    switch (error)
    {
    case ERR_OPENING_USERS_FILE:
        return "Error opening users file!";

    case ERR_OPENING_RESERVATIONS_FILE:
        return "Error opening drivers file!";

    case ERR_OPENING_PASSENGERS_FILE:
        return "Error opening rides file!";

    case ERR_OPENING_FLIGHTS_FILE:
        return "Error opening rides file!";

    case ERR_OPENING_QUERIES_FILE:
        return "Error opening queries file!";

    case ERR_CREATING_DIRECTORY:
        return "Error creating directory";

    case ERR_OPENING_OUTPUT_FILE:
        return "Error opening output file!";

    default:
        return "Unknown error.";
    }
}
