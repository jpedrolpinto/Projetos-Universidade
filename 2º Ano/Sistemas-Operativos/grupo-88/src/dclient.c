#include "../include/client.h"

// Variavel global para armazenar o caminho do pipe do cliente
char client_pipe_path[100];

// Função para enviar uma requisiçao ao servidor
void send_request(Request *request){
    // Abrir o pipe do servidor 
    int server_fd = open(SERVER_PIPE, O_WRONLY);
    if(server_fd == -1){
        perror("Nao foi possivel conectar ao servidor");
        exit(EXIT_FAILURE);
    }

    // pipe do cliente para receber a resposta
    pid_t pid = getpid();
    sprintf(client_pipe_path, "%s%d", CLIENT_PIPE_PREFIX, pid);

    // tentar remover o pipe caso já exista
    unlink(client_pipe_path);

    // Criar pipe com as permissões de leitura e escrita
    if(mkfifo(client_pipe_path, 0666) == -1){
        perror("Erro ao criar o pipe do cliente");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // adicionar o caminho do pipe do clinte à requisição
    strcat(request->data, " ");
    strcat(request->data, client_pipe_path);

    // Enviar a requisição ao servidor
    if(write(server_fd, request, sizeof(Request)) == -1){
        perror("Erro ao enviar a requisição");
        close(server_fd);
        unlink(client_pipe_path);
        exit(EXIT_FAILURE);
    }

    close(server_fd);
}

// Funçao para receber a resposta do servidor
void receive_response(int client_fd, char *response){
    if(read(client_fd, response, MAX_RESPONSE_SIZE) == -1){
        perror("Erro ao receber resposta");
        close(client_fd);
        unlink(client_pipe_path);
        exit(EXIT_FAILURE);
    }
}

// Funçao para adicionar um cdocumento
void add_document(const char *title, const char *authors, const char *year, const char *path){
    Request request;
    char response[MAX_RESPONSE_SIZE] = {0};

    request.type = ADD_DOCUMENT;
    sprintf(request.data, "%s;%s;%s;%s", title, authors, year, path);

    send_request(&request);

    // Abrir o pipe do cliente para ler a resposta
    int client_fd = open(client_pipe_path, O_RDONLY);
    if(client_fd == -1){
        perror("Erro ao abrir o pipe do cliente");
        unlink(client_pipe_path);
        exit(EXIT_FAILURE);
    }

    receive_response(client_fd, response);
    printf("%s\n", response);

    close(client_fd);
    unlink(client_pipe_path);
}

// Função para consultar um documento
void consult_document(const char *key) {
    Request request;
    char response[MAX_RESPONSE_SIZE] = {0};

    request.type = CONSULT_DOCUMENT;
    strcpy(request.data, key);

    send_request(&request);

    int client_fd = open(client_pipe_path, O_RDONLY);
    if (client_fd == -1) {
        perror("Erro ao abrir o pipe do cliente");
        unlink(client_pipe_path);
        exit(EXIT_FAILURE);
    }

    receive_response(client_fd, response);
    printf("%s\n", response);

    close(client_fd);
    unlink(client_pipe_path);
}

// Função para remover um documento
void delete_document(const char *key) {
    Request request;
    char response[MAX_RESPONSE_SIZE] = {0};

    request.type = DELETE_DOCUMENT;
    strcpy(request.data, key);

    send_request(&request);

    int client_fd = open(client_pipe_path, O_RDONLY);
    if (client_fd == -1) {
        perror("Erro ao abrir o pipe do cliente");
        unlink(client_pipe_path);
        exit(EXIT_FAILURE);
    }

    receive_response(client_fd, response);
    printf("%s\n", response);

    close(client_fd);
    unlink(client_pipe_path);
}

// Função para contar linhas com palavra-chave
void count_lines(const char *key, const char *keyword) {
    Request request;
    char response[MAX_RESPONSE_SIZE] = {0};

    request.type = LINE_COUNT;
    sprintf(request.data, "%s;%s", key, keyword);

    send_request(&request);

    int client_fd = open(client_pipe_path, O_RDONLY);
    if (client_fd == -1) {
        perror("Erro ao abrir o pipe do cliente");
        unlink(client_pipe_path);
        exit(EXIT_FAILURE);
    }

    receive_response(client_fd, response);
    printf("%s\n", response);

    close(client_fd);
    unlink(client_pipe_path);
}

// Função para pesquisar documentos com palavra-chave
void search_keyword(const char *keyword, int num_processes) {
    Request request;
    char response[MAX_RESPONSE_SIZE] = {0};

    if (num_processes > 0) {
        request.type = PARALLEL_SEARCH;
        sprintf(request.data, "%s;%d", keyword, num_processes);
    } else {
        request.type = SEARCH_KEYWORD;
        strcpy(request.data, keyword);
    }

    send_request(&request);

    int client_fd = open(client_pipe_path, O_RDONLY);
    if (client_fd == -1) {
        perror("Erro ao abrir o pipe do cliente");
        unlink(client_pipe_path);
        exit(EXIT_FAILURE);
    }

    receive_response(client_fd, response);
    printf("%s\n", response);

    close(client_fd);
    unlink(client_pipe_path);
}

// Função para desligar o servidor
void shutdown_server() {
    Request request;
    char response[MAX_RESPONSE_SIZE] = {0};

    request.type = SHUTDOWN;
    request.data[0] = '\0';

    send_request(&request);

    // Tentar abrir o pipe do cliente
    int client_fd = open(client_pipe_path, O_RDONLY | O_NONBLOCK);
    if (client_fd == -1) {
        // Se não conseguir abrir, assumir que o servidor está desligando
        printf("Server is shutting down\n");
        unlink(client_pipe_path);
        return;
    }

    // Configurar timeout
    fd_set read_fds;
    struct timeval timeout;
    
    FD_ZERO(&read_fds);
    FD_SET(client_fd, &read_fds);
    
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    int ready = select(client_fd + 1, &read_fds, NULL, NULL, &timeout);
    
    if (ready == -1) {
        perror("Erro no select");
    } else if (ready == 0) {
        printf("Server is shutting down (timeout)\n");
    } else {
        receive_response(client_fd, response);
        printf("%s\n", response);
    }

    close(client_fd);
    unlink(client_pipe_path);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s [-a|-c|-d|-l|-s|-f] [argumentos]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (strcmp(argv[1], "-a") == 0) {
        if (argc != 6) {
            fprintf(stderr, "Uso: %s -a \"título\" \"autores\" \"ano\" \"caminho\"\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        add_document(argv[2], argv[3], argv[4], argv[5]);
    }
    else if (strcmp(argv[1], "-c") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Uso: %s -c \"chave\"\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        consult_document(argv[2]);
    }
    else if (strcmp(argv[1], "-d") == 0) {
        if (argc != 3) {
            fprintf(stderr, "Uso: %s -d \"chave\"\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        delete_document(argv[2]);
    }
    else if (strcmp(argv[1], "-l") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Uso: %s -l \"chave\" \"palavra-chave\"\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        count_lines(argv[2], argv[3]);
    }
    else if (strcmp(argv[1], "-s") == 0) {
        if (argc == 3) {
            search_keyword(argv[2], 0);
        }
        else if (argc == 4) {
            int num_processes = atoi(argv[3]);
            search_keyword(argv[2], num_processes);
        }
        else {
            fprintf(stderr, "Uso: %s -s \"palavra-chave\" [num_processos]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }
    else if (strcmp(argv[1], "-f") == 0) {
        if (argc != 2) {
            fprintf(stderr, "Uso: %s -f\n", argv[0]);
            exit(EXIT_FAILURE);
        }
        shutdown_server();
    }
    else {
        fprintf(stderr, "Opção desconhecida: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    return 0;
}
