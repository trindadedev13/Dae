#include "kilate/vector.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kilate/error.h"

klt_vector* klt_vector_make(size_t itemSize) {
  klt_vector* self = malloc(sizeof(klt_vector));
  if (itemSize == 0)
    klt_error_fatal("Item size cant be 0");
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

void klt_vector_delete(klt_vector* self) {
  if (self == NULL)
    klt_error_fatal("Vector is null");
  if (self == NULL)
    return;
  free(self->data);
  free(self);
}

const void* klt_vector_get(klt_vector* self, size_t index) {
  if (self == NULL)
    klt_error_fatal("Vector is null");
  if (index >= self->size)
    klt_error_fatal("Index can't be higher or equal vector size.");

  return (char*)(self->data) + index * self->itemSize;
}

void klt_vector_reserve(klt_vector* self, const size_t size) {
  if (self == NULL)
    klt_error_fatal("Vector is null");
  if (self->capacity <= size) {
    self->data = realloc(self->data, size * self->itemSize);
    memset((char*)self->data + self->capacity * self->itemSize, 0,
           (size - self->capacity) * self->itemSize);
    self->capacity = size;
  }
}

void klt_vector_set(klt_vector* self, size_t index, const void* item) {
  if (self == NULL)
    klt_error_fatal("Vector is null");
  if (index >= self->size)
    klt_error_fatal("Index can't be higher or equal vector size.");

  memcpy((char*)self->data + index * self->itemSize, item, self->itemSize);
}

void klt_vector_insert(klt_vector* self, size_t index, const void* item) {
  if (self == NULL)
    klt_error_fatal("Vector is null");
  if (!(index <= self->size))
    klt_error_fatal("Index can't be higher than vector size");

  if (self->capacity <= self->size) {
    klt_vector_reserve(self, 2 * self->capacity);
  }
  memmove((char*)self->data + (index + 1) * self->itemSize,
          (char*)self->data + index * self->itemSize,
          (self->size - index) * self->itemSize);

  self->size++;
  klt_vector_set(self, index, item);
}

void klt_vector_push_back(klt_vector* self, const void* item) {
  klt_vector_insert(self, self->size, item);
}

void klt_vector_remove(klt_vector* self, size_t index) {
  if (self == NULL)
    klt_error_fatal("Vector is null");
  if (index >= self->size)
    klt_error_fatal("Index can't be higher or equal vector size.");

  if (index < self->size - 1) {
    memmove((char*)self->data + index * self->itemSize,
            (char*)self->data + (index + 1) * self->itemSize,
            (self->size - index - 1) * self->itemSize);
  }
  self->size--;
}