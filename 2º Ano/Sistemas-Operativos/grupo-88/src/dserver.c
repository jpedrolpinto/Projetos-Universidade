#include "../include/server.h"

// Variáveis globais
char document_folder[256];
Document *documents = NULL;
int document_count = 0;
int next_key = 1;
Cache cache;
int running = 1;

// Inicializar o servidor
void init_server(const char *folder, int cache_size) {
    // Armazenar pasta de documentos
    strcpy(document_folder, folder);

    // Inicializar a cache
    init_cache(cache_size);

    // Carregar documentos do disco
    load_documents();

    // Criar pipe do servidor
    unlink(SERVER_PIPE);
    if (mkfifo(SERVER_PIPE, 0666) == -1) {
        perror("Erro ao criar o pipe do servidor");
        exit(EXIT_FAILURE);
    }

    printf("Servidor iniciado. Pasta de documentos: %s, Tamanho da cache: %d\n", 
           document_folder, cache_size);
}

// lidar com requisições dos clientes
void handle_client_request(int server_fd){
    Request request;
    char response[MAX_RESPONSE_SIZE] = {0};
    char client_pipe_path[100] = {0};

    // Ler a requisição
    if (read(server_fd, &request, sizeof(Request)) == -1) {
        perror("Erro ao ler requisição");
        return;
    }

    // Extrair o caminho do pipe do cliente da requisição
    char *last_space = strrchr(request.data, ' ');
    if (last_space != NULL) {
        strcpy(client_pipe_path, last_space + 1);
        *last_space = '\0';  // Remover o caminho do pipe dos dados
    }

    // Processar a requisição
    process_request(&request, client_pipe_path, response);
}

// processar a requisição e gerar a resposta
void process_request(Request *request, char *client_pipe_path, char *response){
    switch (request->type)
    {
    case ADD_DOCUMENT:
        add_document_handler(request->data, response);
        break;
    case CONSULT_DOCUMENT:
        consult_document_handler(request->data, response);
        break;
    case DELETE_DOCUMENT:
        delete_document_handler(request->data, response);
        break;
    case LINE_COUNT:
        count_lines_handler(request->data, response);
        break;
    case SEARCH_KEYWORD:
        search_keyword_handler(request->data, response);
        break;
    case PARALLEL_SEARCH:
        parallel_search_handler(request->data, response);
        break;
    case SHUTDOWN:
        shutdown_handler(response);
        break;
    default:
        strcpy(response, "Tipo de requisição desconhecido");
        break;
    }

    // enviar a resposta ao cliente
    int client_fd = open(client_pipe_path, O_WRONLY);
    if (client_fd == -1) {
        perror("Erro ao abrir pipe do cliente");
        return;
    }

    if (write(client_fd, response, strlen(response) + 1) == -1) {
        perror("Erro ao enviar resposta");
    }

    close(client_fd);
}

// Manipulador para adicionar documento
int add_document_handler(char *data, char *response) {
    char title[MAX_TITLE_SIZE] = {0};
    char authors[MAX_AUTHORS_SIZE] = {0};
    char year[MAX_YEAR_SIZE + 1] = {0};
    char path[MAX_PATH_SIZE] = {0};

    // Extrair os campos dos dados recebidos
    if (sscanf(data, "%[^;];%[^;];%[^;];%[^;]", title, authors, year, path) != 4) {
        strcpy(response, "Erro: Formato de dados inválido");
        return -1;
    }

    // verificar se o arquivo existe
    char *full_path = get_document_path(path);
    if (!file_exists(full_path)) {
        sprintf(response, "Erro: O arquivo %s não existe", path);
        free(full_path);
        return -1;
    }
    free(full_path);

    // alocar memória para novo documento
    documents = realloc(documents, (document_count + 1) * sizeof(Document));
    if (!documents) {
        strcpy(response, "Erro: Falha na alocação de memória");
        return -1;
    }

    // preencher estrutura do documento
    Document new_doc;
    new_doc.key = next_key++;
    
    strncpy(new_doc.title, title, MAX_TITLE_SIZE - 1);
    new_doc.title[MAX_TITLE_SIZE - 1] = '\0';

    strncpy(new_doc.authors, authors, MAX_AUTHORS_SIZE - 1);
    new_doc.authors[MAX_AUTHORS_SIZE - 1] = '\0';

    strncpy(new_doc.year, year, MAX_YEAR_SIZE);
    new_doc.year[MAX_YEAR_SIZE] = '\0';

    strncpy(new_doc.path, path, MAX_PATH_SIZE - 1);
    new_doc.path[MAX_PATH_SIZE - 1] = '\0';
    
    printf("DEBUG: Adicionando documento com chave %d\n", new_doc.key);
    documents[document_count++] = new_doc;

    // adicionar à cache
    add_to_cache(&new_doc);

    // persistir altrações
    save_documents();

    // Preparar resposta
    sprintf(response, "Document %d indexed", new_doc.key);
    return new_doc.key;
}

// Manipulador para consultar documento
void consult_document_handler(char *data, char *response) {
    int key = atoi(data);
    printf("DEBUG: Consultando documento com key: %d\n", key);
    
    // Procurar na cache primeiro
    Document *doc = get_from_cache(key);

    printf("DEBUG: Documento está na cache? %s\n", doc ? "Sim" : "Não");
    
    // Se não estiver na cache, procurar no array
    if (!doc) {
        for (int i = 0; i < document_count; i++) {
            if (documents[i].key == key) {
                doc = &documents[i];
                printf("DEBUG: Documento %d tem key %d\n", i, documents[i].key);
                // Adicionar à cache
                add_to_cache(doc);
                break;
            }
        }
    }
    
    if (doc) {
        printf("DEBUG: Documento encontrado - Título: '%s', Autores: '%s', Ano: '%s', Caminho: '%s'\n", 
            doc->title, doc->authors, doc->year, doc->path);
        sprintf(response, "Title: %s\nAuthors: %s\nYear: %s\nPath: %s", 
                doc->title, doc->authors, doc->year, doc->path);
    } else {
        sprintf(response, "Erro: Documento com chave %d não encontrado", key);
    }
}

// Manipulador para remover documento
void delete_document_handler(char *data, char *response) {
    int key = atoi(data);
    int found = 0;

    // Remover da cache
    remove_from_cache(key);

    // Remover do array
    for(int i = 0; i < document_count; i++){
        if(documents[i].key == key){
            // Mover o ultimo elemento para preencher o espaço
            if(i < document_count - 1){
                documents[i] = documents[document_count - 1];
            }
            document_count--;
            found = 1;
            break;
        }
    }

    if(found){
        // Persistir alterações
        save_documents();
        printf("DEBUG: Documento %d removido, %d documentos restantes.\n", key, document_count);
        sprintf(response, "Index entry %d deleted", key);
    } else {
        sprintf(response, "Erro: Documento com chave %d nao encontrado", key);
    }
}

// Manipulador para contar linhas com palavra-chave
void count_lines_handler(char *data, char *response) {
    char key_str[MAX_KEY_SIZE] = {0};
    char keyword[MAX_KEYWORD_SIZE] = {0};

    // Analisador dados
    sscanf(data, "%[^;];%s", key_str, keyword);
    int key = atoi(key_str);

    // Encontrar o documento
    Document *doc = NULL;
    for(int i = 0; i < document_count; i++){
        if(documents[i].key == key){
            doc = &documents[i];
            break;
        }
    }

    if(!doc){
        sprintf(response, "Erro: Documento com chave %d não encontrado", key);
        return;
    }

    // Obter caminho completo
    char *full_path = get_document_path(doc->path);

    // Contar linhas
    int count = count_lines_with_keyword(full_path, keyword);
    free(full_path);

    // Preparar resposta
    sprintf(response, "%d", count);
}

// Manipulador para pesquisar palavra-chave
void search_keyword_handler(char *data, char *response) {
    char keyword[MAX_KEYWORD_SIZE] = {0};
    strcpy(keyword, data);
    
    int results[10000] = {0};  
    int result_count = 0;
    
    // Realizar pesquisa
    search_keyword_in_documents(keyword, results, &result_count);
    
    // Usar formatação dinâmica
    char *formatted_results = format_results_dynamic(results, result_count);
    
    // Copiar para a resposta (até ao limite do buffer)
    strncpy(response, formatted_results, MAX_RESPONSE_SIZE - 1);
    response[MAX_RESPONSE_SIZE - 1] = '\0'; // Garantir terminação
    
    // Libertar memória alocada
    free(formatted_results);
}

// Manipulador para pesquisa paralela
void parallel_search_handler(char *data, char *response) {
    char keyword[MAX_KEYWORD_SIZE] = {0};
    int num_processes = 1;
    
    // Analisar dados
    sscanf(data, "%[^;];%d", keyword, &num_processes);
    
    if (num_processes <= 0) {
        num_processes = 1;
    }
    
    // Limitar número de processos ao número de documentos
    if (num_processes > document_count) {
        num_processes = document_count;
    }

    // Dividir trabalho entre processos
    int docs_per_process = document_count / num_processes;
    int remaining = document_count % num_processes;

    // Criar pipes para comunicação entre processos filhos e pai
    int pipes[num_processes][2];
    for (int i = 0; i < num_processes; i++) {
        if (pipe(pipes[i]) == -1) {
            perror("Erro ao criar pipe");
            sprintf(response, "Erro ao criar pipes para pesquisa paralela");
            return;
        }
    }

    // Criar processos filhos
    pid_t pids[num_processes];
    for (int i = 0; i < num_processes; i++) {
        pids[i] = fork();
        
        if (pids[i] == -1) {
            // Erro ao criar processo
            perror("Erro ao criar processo filho");
            sprintf(response, "Erro ao criar processos para pesquisa paralela");
            return;
        }
        else if (pids[i] == 0) {
            // Processo filho
            close(pipes[i][0]); // Fechar extremidade de leitura
            
            // Calcular intervalo de documentos para este processo
            int start = i * docs_per_process;
            int end = start + docs_per_process;
            if (i == num_processes - 1) {
                end += remaining;
            }
            
            // Buffer para resultados
            int results[1000] = {0};
            int result_count = 0;
            
            // Pesquisar nos documentos atribuídos
            for (int j = start; j < end; j++) {
                char *full_path = get_document_path(documents[j].path);
                
                // Se o documento contém a palavra-chave, adicionar à lista
                int count = count_lines_with_keyword(full_path, keyword);
                free(full_path);
                
                if (count > 0) {
                    results[result_count++] = documents[j].key;
                }
            }
            
            // Enviar número de resultados
            write(pipes[i][1], &result_count, sizeof(int));
            
            // Enviar resultados
            if (result_count > 0) {
                write(pipes[i][1], results, result_count * sizeof(int));
            }
            
            close(pipes[i][1]);
            exit(0);
        }
        else {
            // Processo pai
            close(pipes[i][1]); // Fechar extremidade de escrita
        }
    }
    
    // Processo pai: recolher resultados
    int all_results[5000] = {0};
    int total_results = 0;
    
    for (int i = 0; i < num_processes; i++) {
        int result_count;
        if (read(pipes[i][0], &result_count, sizeof(int)) > 0) {
            int results[result_count];
            read(pipes[i][0], results, result_count * sizeof(int));
            
            for (int j = 0; j < result_count; j++) {
                all_results[total_results++] = results[j];
            }
        }
        close(pipes[i][0]);
    }
    
    // Esperar por todos os processos filhos
    for (int i = 0; i < num_processes; i++) {
        waitpid(pids[i], NULL, 0);
    }
    
    // Usar formatação dinâmica para a resposta
    char *formatted_results = format_results_dynamic(all_results, total_results);
    
    // Copiar para a resposta (até ao limite do buffer)
    strncpy(response, formatted_results, MAX_RESPONSE_SIZE - 1);
    response[MAX_RESPONSE_SIZE - 1] = '\0'; // Garantir terminação
    
    // Libertar memória alocada
    free(formatted_results);
}

// Manipulador para desligar o servidor
void shutdown_handler(char *response) {
    strcpy(response, "Server is shutting down");
    save_documents();
    running = 0;
    printf("DEBUG: Shutdown solicitado. Flag de shutdown criado.\n");
}

// Inicializar a cache
void init_cache(int size) {
    cache.capacity = size;
    cache.size = 0;
    cache.entries = (Document*)malloc(size * sizeof(Document));
    cache.usage_count = (int*)malloc(size * sizeof(int));
    
    if (!cache.entries || !cache.usage_count) {
        perror("Erro ao alocar memória para cache");
        exit(EXIT_FAILURE);
    }
    
    // Inicializar contadores de uso
    for (int i = 0; i < size; i++) {
        cache.usage_count[i] = 0;
    }
}

// Obter documento da cache
Document* get_from_cache(int key) {
    for (int i = 0; i < cache.size; i++) {
        if (cache.entries[i].key == key) {
            // Incrementar contador de uso
            cache.usage_count[i]++;
            return &cache.entries[i];
        }
    }
    return NULL;
}

// Adicionar documento à cache
void add_to_cache(Document *doc) {
    // Se a cache não estiver cheia
    if (cache.size < cache.capacity) {
        cache.entries[cache.size] = *doc;
        cache.usage_count[cache.size] = 1;
        cache.size++;
    }
    else {
        // Cache cheia, substituir usando política LFU (Least Frequently Used)
        int least_used_index = 0;
        for (int i = 1; i < cache.capacity; i++) {
            if (cache.usage_count[i] < cache.usage_count[least_used_index]) {
                least_used_index = i;
            }
        }
        cache.entries[least_used_index] = *doc;
        cache.usage_count[least_used_index] = 1;
    }
}

// Remover documento da cache
void remove_from_cache(int key) {
    for (int i = 0; i < cache.size; i++) {
        if (cache.entries[i].key == key) {
            // Remover movendo o último item para esta posição
            if (i < cache.size - 1) {
                cache.entries[i] = cache.entries[cache.size - 1];
                cache.usage_count[i] = cache.usage_count[cache.size - 1];
            }
            cache.size--;
            break;
        }
    }
}

// Função para formatar resultados de pesquisa com alocação dinâmica de memória
char *format_results_dynamic(int *results, int result_count) {
    if (result_count <= 0) {
        char *empty_result = strdup("[]");
        return empty_result;
    }
    
    // Estimar tamanho inicial (aproximadamente 10 bytes por resultado)
    size_t initial_size = result_count * 10 + 10;
    char *buffer = malloc(initial_size);
    
    if (!buffer) {
        perror("Erro ao alocar memória para resultados");
        return strdup("[]"); // Retornar resultado vazio em caso de erro
    }
    
    // Iniciar a formatação
    strcpy(buffer, "[");
    size_t used = 1;
    size_t current_size = initial_size;
    
    for (int i = 0; i < result_count; i++) {
        // Criar string temporária para este ID
        char temp[32]; // Buffer temporário grande o suficiente para um ID e vírgula
        sprintf(temp, "%d", results[i]);
        
        // Verificar se precisamos mais espaço
        size_t needed = used + strlen(temp) + 3; // +3 para ", " e possível "]"
        if (needed >= current_size) {
            // Dobrar o tamanho do buffer
            current_size *= 2;
            char *new_buffer = realloc(buffer, current_size);
            if (!new_buffer) {
                perror("Erro ao realocar memória para resultados");
                free(buffer);
                return strdup("[]"); // Retornar resultado vazio em caso de erro
            }
            buffer = new_buffer;
        }
        
        // Adicionar o ID ao buffer
        strcat(buffer, temp);
        used += strlen(temp);
        
        // Adicionar vírgula se não for o último item
        if (i < result_count - 1) {
            strcat(buffer, ", ");
            used += 2;
        }
    }
    
    // Adicionar colchete de fechamento
    strcat(buffer, "]");
    
    return buffer;
}

// Carregar documentos do disco
void load_documents() {
    char index_path[512];
    sprintf(index_path, "%s/index.dat", document_folder);
    printf("DEBUG: Tentando carregar documentos de: %s\n", index_path);

    int fd = open(index_path, O_RDONLY);
    if (fd == -1) {
        if (errno == ENOENT) {
            printf("DEBUG: Arquivo de índice não encontrado, iniciando com 0 documentos\n");
        } else {
            perror("DEBUG: Erro ao abrir arquivo de índice");
        }
        document_count = 0;
        next_key = 1;
        return;
    }
    
    // Ler contagem de documentos
    if (read(fd, &document_count, sizeof(int)) == -1) {
        perror("Erro ao ler contagem de documentos");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    // Ler próxima chave
    if (read(fd, &next_key, sizeof(int)) == -1) {
        perror("Erro ao ler próxima chave");
        close(fd);
        exit(EXIT_FAILURE);
    }
    
    // Alocar memória para documentos
    if (document_count > 0) {
        documents = (Document*)malloc(document_count * sizeof(Document));
        if (!documents) {
            perror("Erro ao alocar memória para documentos");
            close(fd);
            exit(EXIT_FAILURE);
        }
        
        // Ler documentos
        size_t bytes_read = read(fd, documents, document_count * sizeof(Document));
        printf("DEBUG: Lidos %zu bytes de %zu esperados\n", 
               bytes_read, document_count * sizeof(Document));
        if (bytes_read != document_count * sizeof(Document)) {
            perror("Erro ao ler documentos");
            close(fd);
            exit(EXIT_FAILURE);
        }
    } else {
        documents = NULL;  // Garantir que documents seja NULL quando não há documentos
    }
    
    close(fd);
    
    printf("Carregados %d documentos do disco.\n", document_count);
    
    // Limpar completamente a cache e recarregar
    cache.size = 0;
    // Adicionar documentos à cache
    for (int i = 0; i < document_count && i < cache.capacity; i++) {
        add_to_cache(&documents[i]);
    }
}

// Salvar documentos em disco
void save_documents() {
    char index_path[512];
    sprintf(index_path, "%s/index.dat", document_folder);
    
    // Remover arquivo existente antes de criar um novo
    unlink(index_path);
    
    int fd = open(index_path, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Erro ao criar arquivo de índice");
        return;
    }
    
    // Escrever contagem de documentos
    if (write(fd, &document_count, sizeof(int)) == -1) {
        perror("Erro ao escrever contagem de documentos");
        close(fd);
        return;
    }
    
    // Escrever próxima chave
    if (write(fd, &next_key, sizeof(int)) == -1) {
        perror("Erro ao escrever próxima chave");
        close(fd);
        return;
    }
    
    // Escrever documentos apenas se houver algum
    if (document_count > 0 && documents != NULL) {
        ssize_t bytes_written = write(fd, documents, document_count * sizeof(Document));
        if (bytes_written == -1) {
            perror("Erro ao escrever documentos");
            close(fd);
            return;
        }
        
        // Verificar se todos os bytes foram escritos
        if (bytes_written != document_count * sizeof(Document)) {
            fprintf(stderr, "ERRO: Foram escritos apenas %zd bytes de %zu esperados\n", 
                   bytes_written, document_count * sizeof(Document));
        }
    }
    
    close(fd);
    
    printf("Salvos %d documentos em disco.\n", document_count);
}

// Obter caminho completo do documento
char *get_document_path(const char *relative_path) {
    char *full_path = malloc(512);
    if (!full_path) {
        perror("Erro ao alocar memória para caminho");
        exit(EXIT_FAILURE);
    }
    
    sprintf(full_path, "%s/%s", document_folder, relative_path);
    return full_path;
}

// Verificar se um arquivo existe
bool file_exists(const char *path) {
    struct stat st;
    return stat(path, &st) == 0;
}

// Contar linhas com palavra-chave (usando pipe e processos)
int count_lines_with_keyword(const char *path, const char *keyword) {
    int pipe_fd[2];
    if (pipe(pipe_fd) == -1) {
        perror("Erro ao criar pipe");
        return -1;
    }
    
    pid_t pid = fork();
    if (pid == -1) {
        perror("Erro ao criar processo");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }
    
    if (pid == 0) {
        // Processo filho
        close(pipe_fd[0]); // Fechar extremidade de leitura
        
        // Redirecionar stdout para o pipe
        dup2(pipe_fd[1], STDOUT_FILENO);
        close(pipe_fd[1]);
        
        // Executar grep para encontrar a palavra-chave
        execlp("grep", "grep", "-c", keyword, path, NULL);
        
        // Se execlp falhar
        perror("Erro ao executar grep");
        exit(EXIT_FAILURE);
    } else {
        // Processo pai
        close(pipe_fd[1]); // Fechar extremidade de escrita
        
        char buffer[32] = {0};
        ssize_t bytes_read = read(pipe_fd[0], buffer, sizeof(buffer) - 1);
        close(pipe_fd[0]);
        
        // Esperar pelo processo filho
        waitpid(pid, NULL, 0);
        
        if (bytes_read > 0) {
            return atoi(buffer);
        }
        
        return 0;
    }
}

// Pesquisar palavra-chave em documentos
void search_keyword_in_documents(const char *keyword, int *results, int *result_count) {
    *result_count = 0;
    
    for (int i = 0; i < document_count; i++) {
        char *full_path = get_document_path(documents[i].path);
        
        // Se o documento contém a palavra-chave, adicionar à lista
        int count = count_lines_with_keyword(full_path, keyword);
        free(full_path);
        
        if (count > 0) {
            results[(*result_count)++] = documents[i].key;
        }
    }
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s pasta_documentos tamanho_cache\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Inicializar servidor
    init_server(argv[1], atoi(argv[2]));

    while (running) {
        Request request;
        char client_pipe_path[100] = {0};
        char response[MAX_RESPONSE_SIZE] = {0};

        int server_fd = open(SERVER_PIPE, O_RDONLY);
        if (server_fd == -1) {
            perror("Erro ao abrir pipe do servidor");
            unlink(SERVER_PIPE);
            exit(EXIT_FAILURE);
        }

        if (read(server_fd, &request, sizeof(Request)) == -1) {
            perror("Erro ao ler requisição");
            close(server_fd);
            continue;
        }

        // Extrair caminho do pipe do cliente
        char *last_space = strrchr(request.data, ' ');
        if (last_space != NULL) {
            strcpy(client_pipe_path, last_space + 1);
            *last_space = '\0';
        }

        // Tratar requisições que modificam estado diretamente no processo principal
        if (request.type == ADD_DOCUMENT || request.type == DELETE_DOCUMENT || request.type == SHUTDOWN) {
            process_request(&request, client_pipe_path, response);

            int client_fd = open(client_pipe_path, O_WRONLY);
            if (client_fd != -1) {
                write(client_fd, response, strlen(response) + 1);
                close(client_fd);
            }

            close(server_fd);

            if (request.type == SHUTDOWN) {
                printf("DEBUG: Encerramento solicitado via requisição.\n");
                break;
            }

            continue;
        }

        // Outros pedidos → usar processo filho
        pid_t pid = fork();
        if (pid == -1) {
            perror("Erro ao criar processo filho");
            close(server_fd);
            continue;
        }

        if (pid == 0) {
            // Processo filho
            process_request(&request, client_pipe_path, response);

            int client_fd = open(client_pipe_path, O_WRONLY);
            if (client_fd != -1) {
                write(client_fd, response, strlen(response) + 1);
                close(client_fd);
            }

            close(server_fd);
            exit(0);
        } else {
            // Processo pai
            close(server_fd);
            waitpid(pid, NULL, 0);
        }
    }

    // Limpeza final
    unlink(SERVER_PIPE);
    save_documents();

    if (documents) free(documents);
    if (cache.entries) free(cache.entries);
    if (cache.usage_count) free(cache.usage_count);

    printf("Servidor encerrado.\n");
    return 0;
}
