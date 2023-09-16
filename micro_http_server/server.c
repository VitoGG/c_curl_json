#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define RESPONSE_JSON "{\"versao\": \"2.3.0.beta-1\", \"sucesso\": false, \"pack\": 2360 }"

void handle_request(int client_socket) {
  char response_header[1024] = "HTTP/1.1 200 OK\r\nContent-Length: ";
  char response[1024];

  sprintf(response, "%s%d\r\nContent-Type: application/json\r\n\r\n%s",
          response_header, (int)strlen(RESPONSE_JSON), RESPONSE_JSON);

  send(client_socket, response, strlen(response), 0);
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

    // Lida com a solicitação do cliente
    handle_request(client_socket);
  }

  return 0;
}
