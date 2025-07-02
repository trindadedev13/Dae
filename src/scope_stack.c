#include "kilate/scope_stack.h"

#include <stdlib.h>

#include "kilate/error.h"
#include "kilate/node.h"
#include "kilate/vector.h"

klt_scope_stack* klt_scope_stack_make() {
  klt_scope_stack* stack = malloc(sizeof(klt_scope_stack));
  stack->scopes = klt_vector_make(sizeof(klt_hashmap*));
  return stack;
}

void klt_scope_stack_delete(klt_scope_stack* self) {
  for (size_t i = 0; i < self->scopes->size; i++) {
    klt_hashmap* map = *(klt_hashmap**)klt_vector_get(self->scopes, i);

    // for (size_t i = 0; i < map->itens->size; ++i) {
    // klt_hashitem* item = *(klt_hashitem**)klt_vector_get(map->itens, i);
    // if (item && item->value) {
    // klt_node* node = *(klt_node**)item->value;
    // if (node->type == NODE_VARDEC) {
    // if (node->vardec_n.varName != NULL) free(node->vardec_n.varName);
    // if (node->vardec_n.varType != NULL) free(node->vardec_n.varType);
    // if (node != NULL) free(node);
    // }
    // }
    // }

    klt_hash_map_delete(map);
  }
  klt_vector_delete(self->scopes);
  free(self);
}

void klt_scope_stack_push(klt_scope_stack* self) {
  klt_hashmap* map = klt_hash_map_make(sizeof(klt_node*));
  klt_vector_push_back(self->scopes, &map);
}

void klt_scope_stack_pop(klt_scope_stack* self) {
  if (self->scopes->size == 0) {
    klt_error_fatal("klt_scope_stack underflow on pop!");
  }

  klt_hashmap* map =
      *(klt_hashmap**)klt_vector_get(self->scopes, self->scopes->size - 1);
  for (size_t i = 0; i < map->itens->size; ++i) {
    klt_hashitem* item = *(klt_hashitem**)klt_vector_get(map->itens, i);
    if (item != NULL && item->value != NULL) {
      klt_node* node = *(klt_node**)item->value;
      if (node->type == NODE_VARDEC) {
        if (node->vardec_n.varName != NULL)
          free(node->vardec_n.varName);
        if (node->vardec_n.varType != NULL)
          free(node->vardec_n.varType);
        if (node != NULL)
          free(node);
      }
    }
  }
  klt_hash_map_delete(map);
  klt_vector_remove(self->scopes, self->scopes->size - 1);
}

void klt_scope_stack_set(klt_scope_stack* self, klt_str name, klt_node* value) {
  klt_hashmap* map =
      *(klt_hashmap**)klt_vector_get(self->scopes, self->scopes->size - 1);
  klt_hash_map_put(map, name, &value);
}

klt_node* klt_scope_stack_get(klt_scope_stack* self, klt_str name) {
  if (self->scopes->size == 0)
    return NULL;

  for (size_t i = self->scopes->size; i-- > 0;) {
    klt_hashmap* map = *(klt_hashmap**)klt_vector_get(self->scopes, i);
    klt_node** found = (klt_node**)klt_hash_map_get(map, name);
    if (found != NULL) {
      return *found;
    }
  }
  return NULL;
}