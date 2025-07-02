#ifndef __NODE_H__
#define __NODE_H__

#include "kilate/lexer.h"
#include "kilate/string.h"
#include "kilate/vector.h"

typedef enum {
  NODE_FUNCTION,
  NODE_PRINT,
  NODE_CALL,
  NODE_RETURN,
  NODE_VARDEC
} klt_nodetype;

typedef enum {
  NODE_VALUE_TYPE_NUMBER,
  NODE_VALUE_TYPE_STRING,
  NODE_VALUE_TYPE_BOOL,
  NODE_VALUE_TYPE_VAR,
  NODE_VALUE_TYPE_FUNC,
  NODE_VALUE_TYPE_ANY
} klt_node_valuetype;

typedef struct klt_node klt_node;
typedef klt_vector klt_node_vector;

typedef struct {
  klt_str value;
  // klt_str typeStr;
  klt_node_valuetype type;
} klt_node_fnparam;

typedef klt_vector klt_node_fnparam_vector;

struct klt_node {
  klt_nodetype type;

  struct {
    klt_str name;
    klt_str returnType;
    klt_node_vector* body;
    klt_node_fnparam_vector* params;
  } function_n;

  struct {
    klt_str functionName;
    klt_node_fnparam_vector* functionParams;
  } call_n;

  struct {
    klt_node_valuetype returnType;
    void* returnValue;
  } return_n;

  struct {
    klt_str varName;
    klt_str varType;
    klt_node_valuetype varValueType;
    void* varValue;
  } vardec_n;
};

klt_node* klt_function_node_make(klt_str,
                                 klt_str,
                                 klt_node_vector*,
                                 klt_node_fnparam_vector*);

klt_node* klt_call_node_make(klt_str, klt_node_fnparam_vector*);

klt_node* klt_return_node_make(klt_node_valuetype, void*);

klt_node* klt_var_dec_node_make(klt_str, klt_str, klt_node_valuetype, void*);

#endif