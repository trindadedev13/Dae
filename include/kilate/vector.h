#ifndef __VECTOR_H__
#define __VECTOR_H__

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  void* data;
  size_t capacity;
  size_t size;
  size_t itemSize;
} klt_vector;

klt_vector* klt_vector_make(size_t);

void klt_vector_delete(klt_vector*);

const void* klt_vector_get(klt_vector*, size_t);

void klt_vector_set(klt_vector*, const size_t, const void*);

void klt_vector_reserve(klt_vector*, const size_t);

void klt_vector_insert(klt_vector*, const size_t, const void*);

void klt_vector_push_back(klt_vector*, const void*);

void klt_vector_remove(klt_vector*, size_t);

#ifdef __cplusplus
}
#endif

#endif