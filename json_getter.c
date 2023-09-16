#include "json_getter.h"

#include <string.h>

// Função para extrair a versão do JSON
int jsonGetAtributeValue(const char *jsonStr, char *attr, valueGetter *getter,
                         char *type) { 
  memset(getter, 0, sizeof(valueGetter));

  char pivot[32];  // pivo para armazenar a string com a "query" da posicao do
                   // atributo
  sprintf(pivot,
          "\"%s\":", attr);  //  armazena a string ("\"{atributo fodase}\:"")

  const char *attrStart = strstr(jsonStr, pivot);
  printf("tipo: %s\n", attrStart);
  if (attrStart) {
    attrStart += strlen(pivot);

    // Caso o tipo especificado seja um string
    if (strncmp(type, "string", 6) == 0) {
      char attrValue[32];  // Buffer para armazenar o valor da versão
      int numItems = sscanf(attrStart, " \"%31[^\"]\"", attrValue);

      if (numItems == 1) {
        strncpy(
            getter->str, attrValue,
            sizeof(getter->str));  // Copia o valor de "versao" para a estrutura
        getter->str[sizeof(getter->str) - 1] =
            '\0';  // Adiciona um terminador nulo para formuma uma string
        return 1;
      }
      return 0;
    } else if (strncmp(type, "bool", 4) == 0) {
      if (strncmp(attrStart, "true", 4) == 0) {
        getter->booli = 1;
      } else {
        getter->booli = 0;
      }
      return 1;
    } else if (strncmp(type, "num", 3) == 0) { 
      const char *attrEnd = strchr(attrStart, '}');
      
      size_t len = attrEnd - (attrStart + 1);
      char num[len + 1];

      strncpy(num, attrStart + 1, len);
      num[len] = '\0';

      if (num) {
        getter->num = atoi(num);
      }
    }
  }

  return 0;
}