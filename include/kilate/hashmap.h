#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>

#include "kilate/string.h"
#include "kilate/vector.h"

typedef Vector HashMapVector;
typedef Vector HashItemVector;

typedef struct {
  HashItemVector* itens;
  size_t itemSize;
  size_t capacity;
} HashMap;

typedef struct {
  String key;
  void* value;
  void* next;
} HashItem;

HashMap* HashMap_New(size_t);

void HashMap_Delete(HashMap*);

unsigned int HashMap_Hash(HashMap*, String);

void* HashMap_Get(HashMap*, String);

void HashMap_Put(HashMap*, String, void*);

#endif