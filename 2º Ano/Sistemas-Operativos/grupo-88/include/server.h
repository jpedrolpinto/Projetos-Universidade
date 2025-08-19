// include/server.h
#ifndef SERVER_H
#define SERVER_H

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>


// Estrutura de cache para os documentos
typedef struct {
    Document *entries; // array de documentos em cache
    int *usage_count; // contador de uso para cada entrada
    int size; // Tamanho atual 
    int capacity; // Capacidade máxima 
} Cache;

// Funções do servidor
void init_server(const char *document_folder, int cache_size);
void handle_client_request(int server_fd);
void process_request(Request *request, char *client_pipe_path, char *response);
int add_document_handler(char *data, char *response);
void consult_document_handler(char *data, char *response);
void delete_document_handler(char *data, char *response);
void count_lines_handler(char *data, char *response);
void search_keyword_handler(char *data, char *response);
void parallel_search_handler(char *data, char *response);
void shutdown_handler(char *response);

// Funções para persistência
void load_documents();
void save_documents();

// Funções para cache
void init_cache(int size);
Document* get_from_cache(int key);
void add_to_cache(Document *doc);
void remove_from_cache(int key);
char *format_results_dynamic(int *results, int result_count);

// Funções utilitárias
char *get_document_path(const char *relative_path);
bool file_exists(const char *path);
int count_lines_with_keyword(const char *path, const char *keyword);
void search_keyword_in_documents(const char *keyword, int *results, int *result_count);


#endif