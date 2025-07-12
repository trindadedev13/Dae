#ifndef __NATIVE_H__
#define __NATIVE_H__

#include "kilate/environment.h"
#include "kilate/lexer.h"
#include "kilate/node.h"
#include "kilate/parser.h"
#include "kilate/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  klt_environment* env;
  klt_node_fnparam_vector* params;
} klt_native_fndata;

typedef klt_node* (*klt_native_fn)(klt_native_fndata*);

typedef struct {
  klt_str name;
  klt_str_vector* requiredParams;
  klt_native_fn fn;
} klt_native_fnentry;

#define KILATE_NATIVE_REGISTER() void KILATE_NATIVE_REGISTER()

extern klt_node_vector* native_functions;

void klt_native_init();

void klt_native_load_extern();

void klt_native_end();

void klt_native_register_fnentry(klt_native_fnentry*);

void klt_native_register_fn(klt_str, klt_str_vector*, klt_native_fn);

klt_native_fnentry* klt_native_find_function(klt_str);

void klt_native_register_all_functions();

#ifdef __cplusplus
}
#endif

#endif