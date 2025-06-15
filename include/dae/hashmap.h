#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdio.h>

#include "dae/string.h"
#include "dae/vector.h"

typedef struct {
  Vector* itens;
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