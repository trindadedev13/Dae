#include "kilate/node.h"

#include <stdlib.h>
#include <string.h>

#include "kilate/string.h"
#include "kilate/vector.h"

klt_node* klt_function_node_make(klt_str name,
                                 klt_str returnType,
                                 klt_node_vector* body,
                                 klt_node_fnparam_vector* params) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_FUNCTION;
  node->function_n.name = strdup(name);
  if (returnType != NULL) {
    node->function_n.returnType = strdup(returnType);
  } else {
    node->function_n.returnType = NULL;
  }
  node->function_n.body = body;
  node->function_n.params = params;
  return node;
}

klt_node* klt_call_node_make(const klt_str functionName,
                             klt_node_fnparam_vector* functionParams) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_CALL;
  node->call_n.functionName = strdup(functionName);
  node->call_n.functionParams = functionParams;
  return node;
}

klt_node* klt_return_node_make(klt_node_valuetype returnType,
                               void* returnValue) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_RETURN;
  node->return_n.returnType = returnType;
  node->return_n.returnValue = returnValue;
  return node;
}

klt_node* klt_var_dec_node_make(klt_str name,
                                klt_str type,
                                klt_node_valuetype valueType,
                                void* value) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_VARDEC;
  node->vardec_n.varName = strdup(name);
  node->vardec_n.varType = strdup(type);
  node->vardec_n.varValueType = valueType;
  node->vardec_n.varValue = value;
  return node;
}