#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__

#include "kilate/environment.h"
#include "kilate/hashmap.h"
#include "kilate/node.h"
#include "kilate/string.h"
#include "kilate/vector.h"

typedef struct {
  klt_hashmap* functions;
  klt_hashmap* native_functions;
  klt_environment* env;
} klt_interpreter;

typedef enum { IRT_FUNC, IRT_RETURN } klt_interpreter_result_type;

typedef struct {
  void* data;
  klt_interpreter_result_type type;
} klt_interpreter_result;

klt_interpreter* klt_interpreter_make(klt_node_vector*, klt_node_vector*);

void klt_interpreter_delete(klt_interpreter*);

klt_interpreter_result klt_interpreter_run(klt_interpreter*);

klt_interpreter_result klt_interpreter_run_fn(klt_interpreter*,
                                              klt_node*,
                                              klt_str_vector*);

klt_interpreter_result klt_interpreter_run_node(klt_interpreter*, klt_node*);

#endif