#ifndef USERS_CATALOG_H
#define USERS_CATALOG_H


#include <glib.h>

#include "types/user.h"

// HashTable dos users, users é um apontador para a hashtable, 
//e USERS_CATALOG é um type que representa um apontador para a struct.
typedef struct users_catalog 
{
    GHashTable *users;

} *USERS_CATALOG;


USERS_CATALOG create_users_catalog(void);

void insert_user(USERS_CATALOG catalog, USER user);

USER get_user_by_id(USERS_CATALOG catalog, char* user_id);

int tem_prefixo(char *prefix, char *name);

void free_users_catalog(USERS_CATALOG catalog);

#endif