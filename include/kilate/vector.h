#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>

typedef struct {
  void* data;
  size_t capacity;
  size_t size;
  size_t itemSize;
} Vector;

typedef Vector StringVector;

Vector* Vector_New(size_t);

void Vector_Delete(Vector*);

const void* Vector_Get(Vector*, size_t);

void Vector_Set(Vector*, const size_t, const void*);

void Vector_Reserve(Vector*, const size_t);

void Vector_Insert(Vector*, const size_t, const void*);

void Vector_PushBack(Vector*, const void*);

void Vector_Remove(Vector*, size_t);

#endif