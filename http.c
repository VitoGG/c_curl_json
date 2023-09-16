#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <string.h>

typedef struct
{
  char str[4096];
  int bool;
} valueGetter;

// Função de callback para escrever os dados da resposta em um buffer
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t real_size = size * nmemb;
  char *buffer = (char *)userp;
  strcat(buffer, contents);
  return real_size;
}

void jsonGetValue(const char *jsonStr, char *attr, valueGetter *getter, char *type)
{
  memset(getter, 0, sizeof(valueGetter));

  char pivot[32]; // pivo para armazenar a string com a "query" da posicao do atributo 
  sprintf(pivot, "\"%s\":", attr); //  armazena a string ("\"{atributo fodase}\:"")

  const char *attrStart = strstr(jsonStr, pivot);
  if (attrStart)
  {
    attrStart += strlen(pivot);
    const char *attrEnd = strchr(attrStart, '\"');
    if (strncmp(type, "string", 6) == 0)
    {
      char attrValue[32]; // Buffer para armazenar o valor da versão
      int numItems = sscanf(attrStart, " \"%31[^\"]\"", attrValue);

      if (numItems == 1)
      {
        strncpy(getter->str, attrValue, sizeof(getter->str)); // Copia o valor de "versao" para a estrutura
        getter->str[sizeof(getter->str) - 1] = '\0';          // Adiciona um terminador nulo para formuma uma string
      }
    }
    else if (strncmp(type, "bool", 4) == 0)
    {
      if (strncmp(attrStart, "true", 4) == 0)
      {
        getter->bool = 1;
      }
      else
      {
        getter->bool = 0;
      }
    }
  }
}

int main(void)
{
  CURL *curl;
  CURLcode response;

  curl_global_init(CURL_GLOBAL_ALL);

  curl = curl_easy_init();

  if (curl)
  {
    curl_easy_setopt(curl, CURLOPT_URL, "http://localhost:8080/periferico/recuperar-versao");

    char buffer[4096];
    buffer[0] = '\0';

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, buffer);

    response = curl_easy_perform(curl);

    if (response != CURLE_OK)
    {
      fprintf(stderr, "Request failed: %s\n", curl_easy_strerror(response));
    }
    else
    {
      size_t size = strlen(buffer);

      valueGetter versao;

      jsonGetValue(buffer, "versao", &versao, "string");
      printf("versao: %s\n", versao.str);

      valueGetter sucesso;

      jsonGetValue(buffer, "sucesso", &sucesso, "bool");
      printf("SUcesso>>>>>>>> %d\n", sucesso.bool);

      printf("RESPOSTA >>>>%s\n", buffer);
      printf("LENGTH: %zu\n", size);
    }
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  return 0;
}
