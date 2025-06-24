#include "kilate/hashmap.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kilate/string.h"

HashMap* HashMap_New(size_t itemSize) {
  HashMap* hashMap = malloc(sizeof(HashMap));
  hashMap->itemSize = itemSize;
  hashMap->capacity = 64;
  hashMap->itens = Vector_New(sizeof(HashItem*));
  for (int i = 0; i < hashMap->capacity; i++) {
    HashItem* null_ptr = NULL;
    Vector_PushBack(hashMap->itens, &null_ptr);
  }
  return hashMap;
}

void HashMap_Delete(HashMap* self) {
  for (size_t i = 0; i < self->itens->size; ++i) {
    HashItem* item = *(HashItem**)Vector_Get(self->itens, i);
    if (item != NULL) {
      free(item->key);
      free(item->value);
      free(item);
    }
  }
  Vector_Delete(self->itens);
  free(self);
}

unsigned int HashMap_Hash(HashMap* self, String key) {
  assert(self);
  assert(key);
  unsigned int hash = 5381;
  int c;
  while ((c = *key++)) {
    hash = ((hash << 5) + hash) + c;  // hash * 33 + c
  }
  return hash % self->capacity;
}
void* HashMap_Get(HashMap* self, String key) {
  assert(self);
  assert(key);
  unsigned int index = HashMap_Hash(self, key);

  HashItem** itemPtr = (HashItem**)Vector_Get(self->itens, index);
  HashItem* item = *itemPtr;

  while (item) {
    if (String_Equals(item->key, key)) {
      return item->value;
    }
    item = item->next;
  }
  return NULL;
}

void HashMap_Put(HashMap* self, String key, void* value) {
  assert(self);
  assert(key);

  unsigned int index = HashMap_Hash(self, key);
  HashItem** headPtr = (HashItem**)Vector_Get(self->itens, index);
  HashItem* head = *headPtr;

  HashItem* item = head;
  while (item) {
    if (String_Equals(item->key, key)) {
      memcpy(item->value, value, self->itemSize);
      return;
    }
    item = item->next;
  }

  HashItem* newItem = malloc(sizeof(HashItem));
  newItem->key = strdup(key);
  newItem->value = malloc(self->itemSize);
  memcpy(newItem->value, value, self->itemSize);
  newItem->next = head;

  *headPtr = newItem;
}
