#include "kilate/scope_stack.h"

#include <stdlib.h>

#include "kilate/error.h"
#include "kilate/node.h"  // Para Node_Delete
#include "kilate/vector.h"

ScopeStack* ScopeStack_New() {
  ScopeStack* stack = malloc(sizeof(ScopeStack));
  stack->scopes = Vector_New(sizeof(HashMap*));
  return stack;
}

void ScopeStack_Delete(ScopeStack* self) {
  for (size_t i = 0; i < self->scopes->size; i++) {
    HashMap* map = *(HashMap**)Vector_Get(self->scopes, i);

    // for (size_t i = 0; i < map->itens->size; ++i) {
    // HashItem* item = *(HashItem**)Vector_Get(map->itens, i);
    // if (item && item->value) {
    // Node* node = *(Node**)item->value;
    // if (node->type == NODE_VARDEC) {
    // if (node->vardec_n.varName != NULL) free(node->vardec_n.varName);
    // if (node->vardec_n.varType != NULL) free(node->vardec_n.varType);
    // if (node != NULL) free(node);
    // }
    // }
    // }

    HashMap_Delete(map);
  }
  Vector_Delete(self->scopes);
  free(self);
}

void ScopeStack_Push(ScopeStack* self) {
  HashMap* map = HashMap_New(sizeof(Node*));
  Vector_PushBack(self->scopes, &map);
}

void ScopeStack_Pop(ScopeStack* self) {
  if (self->scopes->size == 0) {
    Error_Fatal("ScopeStack underflow on pop!");
  }

  HashMap* map = *(HashMap**)Vector_Get(self->scopes, self->scopes->size - 1);
  for (size_t i = 0; i < map->itens->size; ++i) {
    HashItem* item = *(HashItem**)Vector_Get(map->itens, i);
    if (item != NULL && item->value != NULL) {
      Node* node = *(Node**)item->value;
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
  HashMap_Delete(map);
  Vector_Remove(self->scopes, self->scopes->size - 1);
}

void ScopeStack_Set(ScopeStack* self, String name, Node* value) {
  HashMap* map = *(HashMap**)Vector_Get(self->scopes, self->scopes->size - 1);
  HashMap_Put(map, name, &value);
}

Node* ScopeStack_Get(ScopeStack* self, String name) {
  if (self->scopes->size == 0)
    return NULL;

  for (size_t i = self->scopes->size; i-- > 0;) {
    HashMap* map = *(HashMap**)Vector_Get(self->scopes, i);
    Node** found = (Node**)HashMap_Get(map, name);
    if (found != NULL) {
      return *found;
    }
  }
  return NULL;
}