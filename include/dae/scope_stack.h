#ifndef SCOPE_STACK_H
#define SCOPE_STACK_H

#include "dae/hashmap.h"
#include "dae/node.h"
#include "dae/string.h"

#define MAX_SCOPE_DEPTH 64

typedef struct {
  HashMapVector* scopes;
  size_t count;
} ScopeStack;

ScopeStack* ScopeStack_New();

void ScopeStack_Delete(ScopeStack*);

void ScopeStack_Push(ScopeStack*);

void ScopeStack_Pop(ScopeStack*);

void ScopeStack_Set(ScopeStack*, String, Node*);

Node* ScopeStack_Get(ScopeStack*, String);

#endif