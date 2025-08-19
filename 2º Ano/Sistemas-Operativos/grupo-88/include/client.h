// include/client.h
#ifndef CLIENT_H
#define CLIENT_H

#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

// Funções do cliente
void send_request(Request *request);
void receive_response(int client_fd, char *response);
void add_document(const char *title, const char *authors, const char *year, const char *path);
void consult_document(const char *key);
void delete_document(const char *key);
void count_lines(const char *key, const char *keyword);
void search_keyword(const char *keyword, int num_processes);
void shutdown_server();

#endif