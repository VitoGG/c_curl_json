#ifndef JSON_GETTER_H
#define JSON_GETTER_H

// Estrutura para armazenar os dados JSON
typedef struct {
  char str[4096];
  int booli;
  int num;
} valueGetter;

// Função para extrair a versão do JSON
int jsonGetAtributeValue(const char *jsonStr, char *attr, valueGetter *getter,
                         char *type);

#endif /* JSON_GETTER_H */