#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "json_getter.h"

// Função de callback para escrever os dados da resposta em um buffer
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t real_size = size * nmemb;
  char *buffer = (char *)userp;
  strcat(buffer, contents);
  return real_size;
}

int main(void) {
  CURL *curl;
  CURLcode response;

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();

  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL,
                     "http://localhost:8080/periferico/recuperar-versao");

    char buffer[4096];
    buffer[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

    response = curl_easy_perform(curl);

    if (response != CURLE_OK) {
      fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(response));
    } else {
      size_t size = strlen(buffer);

      valueGetter versao;
      jsonGetAtributeValue(buffer, "versao", &versao, "string");
      printf("Versão >>> %s\n", versao.str);

      valueGetter sucesso;
      jsonGetAtributeValue(buffer, "sucesso", &sucesso, "bool");
      printf("Sucesso >>> %d\n", sucesso.booli);

      valueGetter pack;
      jsonGetAtributeValue(buffer, "pack", &pack, "num");
      printf("Pack >>> %d\n", pack.num);

      // Resposta completa
      printf("RESPOSTA >>>>%s\n", buffer);
      printf("LENGTH: %zu\n", size);
    }
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  return 0;
}
