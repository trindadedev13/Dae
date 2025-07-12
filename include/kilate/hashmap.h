#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <stdio.h>

#include "kilate/string.h"
#include "kilate/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef klt_vector klt_hashmap_vector;
typedef klt_vector klt_hashitem_vector;

typedef struct {
  klt_hashitem_vector* itens;
  size_t itemSize;
  size_t capacity;
} klt_hashmap;

typedef struct {
  klt_str key;
  void* value;
  void* next;
} klt_hashitem;

klt_hashmap* klt_hash_map_make(size_t);

void klt_hash_map_delete(klt_hashmap*);

unsigned int klt_hash_map_hash(klt_hashmap*, klt_str);

void* klt_hash_map_get(klt_hashmap*, klt_str);

void klt_hash_map_put(klt_hashmap*, klt_str, void*);

#ifdef __cplusplus
}
#endif

#endif