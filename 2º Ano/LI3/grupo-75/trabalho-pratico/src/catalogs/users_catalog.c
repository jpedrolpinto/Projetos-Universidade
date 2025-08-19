#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "catalogs/users_catalog.h"

USERS_CATALOG create_users_catalog(void)
{
    USERS_CATALOG new_catalog = malloc(sizeof(struct users_catalog));

    new_catalog->users = g_hash_table_new_full(g_str_hash, g_str_equal, NULL, (GDestroyNotify)free_user); // funçao de hash apartir da key, funçao de comparaçao das key, funçao para libertar

    return new_catalog;
}

void insert_user(USERS_CATALOG catalog, USER user)
{
    g_hash_table_insert(catalog->users, user->id, user);
}

USER get_user_by_id(USERS_CATALOG catalog, char *user_id)
{
    if (catalog == NULL || catalog->users == NULL || user_id == NULL)
        // Tratar casos de catálogo, tabela de hash ou ID de usuário nulos.
        return NULL;

    USER user = (USER)g_hash_table_lookup(catalog->users, (gconstpointer)user_id);
    if (user == NULL)
    {
        // O usuário não foi encontrado. Trate conforme necessário.
    }
    return user;
}

int tem_prefixo(char *prefix, char *name)
{

    size_t tamanho_prefixo = strlen(prefix);
    size_t tamanho_palavra = strlen(name);

    // Verifica se o tamanho da palavra é menor que o tamanho do prefixo
    if (tamanho_palavra < tamanho_prefixo)
        return false;

    // Compara os primeiros tamanho_prefixo caracteres
    return strncmp(name, prefix, tamanho_prefixo) == 0;
}

void free_users_catalog(USERS_CATALOG catalog)
{
    g_hash_table_destroy(catalog->users);
    free(catalog);
}