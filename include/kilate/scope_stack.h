#ifndef __SCOPE_STACK_H__
#define __SCOPE_STACK_H__

#include "kilate/hashmap.h"
#include "kilate/node.h"
#include "kilate/string.h"

typedef struct {
  klt_hashmap_vector* scopes;
  size_t count;
} klt_scope_stack;

klt_scope_stack* klt_scope_stack_make();

void klt_scope_stack_delete(klt_scope_stack*);

void klt_scope_stack_push(klt_scope_stack*);

void klt_scope_stack_pop(klt_scope_stack*);

void klt_scope_stack_set(klt_scope_stack*, klt_str, klt_node*);

klt_node* klt_scope_stack_get(klt_scope_stack*, klt_str);

#endif