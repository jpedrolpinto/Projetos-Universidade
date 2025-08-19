// include/common.h
#ifndef COMMON_H
#define COMMON_H

#define MAX_TITLE_SIZE 200
#define MAX_AUTHORS_SIZE 200
#define MAX_PATH_SIZE 64
#define MAX_YEAR_SIZE 4
#define MAX_REQUEST_SIZE 512
#define MAX_RESPONSE_SIZE 16384
#define MAX_KEY_SIZE 20
#define MAX_KEYWORD_SIZE 100

// Tipos de requisições
typedef enum {
    ADD_DOCUMENT,      // Adicionar documento
    CONSULT_DOCUMENT,  // Consultar documento
    DELETE_DOCUMENT,   // Remover documento
    LINE_COUNT,        // Contar linhas com palavra-chave
    SEARCH_KEYWORD,    // Pesquisar palavra-chave
    PARALLEL_SEARCH,   // Pesquisa paralela
    SHUTDOWN           // Desligar servidor
} RequestType;

// Estrutura de requisição
typedef struct {
    RequestType type;
    char data[MAX_REQUEST_SIZE];
} Request;

// Estrutura para armazenar metadados de documento
typedef struct {
    int key;                       // Identificador único
    char title[MAX_TITLE_SIZE];    // Título
    char authors[MAX_AUTHORS_SIZE];// Autores
    char year[MAX_YEAR_SIZE + 1];  // Ano
    char path[MAX_PATH_SIZE];      // Caminho do arquivo
} Document;

// Nomes para os pipes
#define SERVER_PIPE "/tmp/server_pipe"
#define CLIENT_PIPE_PREFIX "/tmp/client_pipe_"

#endif