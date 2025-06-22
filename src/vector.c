#include "dae/vector.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

Vector* Vector_New(size_t itemSize) {
  Vector* self = malloc(sizeof(Vector));
  assert(itemSize);
  if (self == NULL) {
    printf("Failed to create vector, Out Of Memory.\n");
    return NULL;
  }
  self->itemSize = itemSize;
  self->size = 0;
  self->capacity = 1;
  self->data = calloc(self->itemSize, self->capacity);
  return self;
}

void Vector_Delete(Vector* self) {
  assert(self);
  free(self->data);
  free(self);
}

const void* Vector_Get(Vector* self, size_t index) {
  assert(self);
  assert(index < self->size);

  return (char*)(self->data) + index * self->itemSize;
}

void Vector_Reserve(Vector* self, const size_t size) {
  assert(self);
  if (self->capacity <= size) {
    self->data = realloc(self->data, size * self->itemSize);
    memset((char*)self->data + self->capacity * self->itemSize, 0,
           (size - self->capacity) * self->itemSize);
    self->capacity = size;
  }
}

void Vector_Set(Vector* self, size_t index, const void* item) {
  assert(self);
  assert(index < self->size);

  memcpy((char*)self->data + index * self->itemSize, item, self->itemSize);
}

void Vector_Insert(Vector* self, size_t index, const void* item) {
  assert(self);
  assert(index <= self->size);

  if (self->capacity <= self->size) {
    Vector_Reserve(self, 2 * self->capacity);
  }
  memmove((char*)self->data + (index + 1) * self->itemSize,
          (char*)self->data + index * self->itemSize,
          (self->size - index) * self->itemSize);

  self->size++;
  Vector_Set(self, index, item);
}

void Vector_PushBack(Vector* self, const void* item) {
  Vector_Insert(self, self->size, item);
}

void Vector_Remove(Vector* self, size_t index) {
  assert(self);
  assert(index < self->size);
  if (index < self->size - 1) {
    memmove((char*)self->data + index * self->itemSize,
            (char*)self->data + (index + 1) * self->itemSize,
            (self->size - index - 1) * self->itemSize);
  }
  self->size--;
}