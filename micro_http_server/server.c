#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8081
#define RESPONSE_JSON "{\"versao\": \"2.3.0.beta-1\", \"sucesso\": false}"

int pack = 2360;

// Para tratar uma request simples JSON
void handle_request(int client_socket) {
    char response_header[1024] = "HTTP/1.1 200 OK\r\nContent-Length: ";
    char response[1024];

    sprintf(response, "%s%d\r\nContent-Type: application/json\r\n\r\n{\"pack\": %d, %s}",
        response_header, (int)(strlen(RESPONSE_JSON) + 10), pack, RESPONSE_JSON);

    send(client_socket, response, strlen(response), 0);
    close(client_socket);

    // Incrementa o valor de "pack" após o tratamento da solicitação
    pack++;
}

// Função simples para ler o arquivo
char* read_file(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) return NULL;

    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;
    return string;
}

void handle_html_css(int client_socket) {
    char buffer[2048];
    char *response_header_template = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %d\r\n\r\n";
    char response[512];

    read(client_socket, buffer, sizeof(buffer));

    char path[512];
    sscanf(buffer, "GET %s ", path);

    char *content_type;
    char filepath[512];

    /*
    * Serve todos arquivos necessários para o funcionamento do client
    * TODO: Transformar em uma função com maior dinamismo para hospedar
    */
    if (strcmp(path, "/") == 0 || strcmp(path, "/index.html") == 0) {
        content_type = "text/html";
        strcpy(filepath, "index.html");
    } else if (strcmp(path, "/assets/css/bootstrap.min.css") == 0) {
        content_type = "text/css";
        strcpy(filepath, "assets/css/bootstrap.min.css");
    } 
    else if (strcmp(path, "/assets/js/react.development.js") == 0) { 
        content_type = "text/javascript";
        strcpy(filepath, "assets/js/react.development.js"); 
    } else {
        const char *error_msg = "HTTP/1.1 404 Not Found\r\nContent-Length: 0\r\n\r\n";
        send(client_socket, error_msg, strlen(error_msg), 0);
        return;
    }

    char *file_content = read_file(filepath);
    if (file_content) {
        sprintf(response, response_header_template, content_type, strlen(file_content));
        send(client_socket, response, strlen(response), 0);
        send(client_socket, file_content, strlen(file_content), 0);
        // IMPORTANT
        free(file_content); // libera memoria do conteudo após envialo ao client :))
        // IMPORTANTEE
    } else {
        const char *error_msg = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 0\r\n\r\n";
        send(client_socket, error_msg, strlen(error_msg), 0);
    }
    close(client_socket);   
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_address_len = sizeof(client_address);

    // Cria o socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Erro ao criar o socket");
        exit(1);
    }

    // Configura o endereço do servidor
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    // Liga o socket ao endereço do servidor
    if (bind(server_socket, (struct sockaddr *)&server_address,
            sizeof(server_address)) < 0) {
        perror("Erro ao fazer o bind");
        exit(1);
    }

    // Inicia o servidor e fica esperando por conexões
    if (listen(server_socket, 5) < 0) {
        perror("Erro ao escutar");
        exit(1);
    }

    printf("Servidor HTTP em execução na porta %d...\n", PORT);

    while (1) {
        // Aceita a conexão de um cliente
        client_socket = accept(server_socket, (struct sockaddr *)&client_address,
                            &client_address_len);
        if (client_socket < 0) {
            perror("Erro ao aceitar a conexão");
            exit(1);
        }

        // Lida com request do client (ISSO INCLUI O INDEX.HTML SOLICITANDO OS ARQUIVOS JS E CSS)
        handle_html_css(client_socket);
    }
        

    return 0;
}