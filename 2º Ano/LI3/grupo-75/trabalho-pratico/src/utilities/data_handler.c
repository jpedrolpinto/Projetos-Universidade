#include <ctype.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

void to_lower(char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        string[i] = tolower(string[i]);
    }
}

void to_upper(char *string)
{
    for (int i = 0; string[i] != '\0'; i++)
    {
        string[i] = toupper(string[i]);
    }
}

int string_to_int(char *string)
{
    // return atoi(string);
    int value = 0;

    for (int i = 0; string[i] != '\0'; i++)
    {
        value *= 10;
        value += string[i] - '0';
    }
    return value;
}

int is_valid_int(char *str)
{
    if (!str || *str == '\0')
    {
        return 0; // String vazia ou ponteiro nulo
    }

    // Verifica se há caracteres não numéricos
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] < '0' || str[i] > '9')
        {
            return 0; // Caractere não numérico encontrado
        }
    }
    return 1; // A string representa um número inteiro válido
}

void swap(int *xp, int *yp)
{
    int temp = *xp; // saves in temp the value of xp
    *xp = *yp;      // saves in xp the value of yp
    *yp = temp;     // saves in yp the value of temp(xp)
}

// Selection Sort
void selection_sort(int arr[], int n)
{ // organiza de forma crescente
    int i, j, min_idx;

    for (i = 0; i < n - 1; i++)
    {
        min_idx = i;
        for (j = i + 1; j < n; j++)
            if (arr[j] < arr[min_idx])
                min_idx = j;

        swap(&arr[min_idx], &arr[i]);
    }
}