#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jstdint.h"

typedef enum { LAB, DIR, INS, REG, IMM } TokenType;

typedef struct {
  TokenType type;
  const char* text;
  u64 row;
  u64 col;
} Token;

char* read_entire_file(const char* path) {
  FILE* file = fopen(path, "rb");
  if (!file) {
    perror("Failed to open file");
    return NULL;
  }

  fseek(file, 0, SEEK_END);
  u64 size = ftell(file);
  rewind(file);

  char* buf = malloc(size+1);
  if (!buf) {
    perror("Memory allocation failed");
    fclose(file);
    return NULL;
  }

  size_t bytes = fread(buf, sizeof(char), size, file);
  if (bytes < size) {
    perror("Couldn't read entire file");
    free(buf);
    return NULL;
  }

  return buf;
}

typedef struct {
  void* items;
  size_t size;
  size_t capacity;
} DynamicArray;

void* da_pushback(DynamicArray* da, void* item, size_t item_size) {
  if (da->size + item_size < 2*da->capacity) {
    da->items = realloc(da->items, da->capacity*2);
  } else {
    da->items = realloc(da->items, 2*da->capacity + 2*item_size);
  }
  memcpy(da->items, item, item_size);
  da->size++;
  return da->items;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "ERROR: usage: %s <file>\n", argv[0]);
    return 1;
  }

  const char* path = argv[1];
  char* content = read_entire_file(path);
  size_t size = strlen(content);
  printf("%s: %zu bytes", path, size);

  DynamicArray da_tokens = {0};

  for (size_t i = 0; i < size; i++) {
    if (content[i] == '')
    da_pushback(&da_tokens, )
  }
}
