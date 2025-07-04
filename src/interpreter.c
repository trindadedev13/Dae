#include "kilate/interpreter.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "kilate/environment.h"
#include "kilate/error.h"
#include "kilate/hashmap.h"
#include "kilate/node.h"
#include "kilate/parser.h"

klt_interpreter* klt_interpreter_make(
    klt_node_vector* funcklt_nodes,
    klt_node_vector* nativeFunctionsklt_nodes) {
  assert(funcklt_nodes);
  assert(nativeFunctionsklt_nodes);
  klt_interpreter* interpreter = malloc(sizeof(klt_interpreter));
  interpreter->functions = klt_hash_map_make(sizeof(klt_node*));
  interpreter->nativeFunctions = klt_hash_map_make(sizeof(klt_native_fn));

  // register all funcs
  for (int i = 0; i < funcklt_nodes->size; i++) {
    klt_node** nodePtr = (klt_node**)klt_vector_get(funcklt_nodes, i);
    if (nodePtr != NULL) {
      klt_node* node = *nodePtr;
      if (node->type == NODE_FUNCTION) {
        klt_hash_map_put(interpreter->functions, node->function_n.name, &node);
      }
    }
  }

  for (int i = 0; i < nativeFunctionsklt_nodes->size; i++) {
    klt_native_fnentry** entryPtr =
        (klt_native_fnentry**)klt_vector_get(nativeFunctionsklt_nodes, i);
    if (entryPtr != NULL) {
      klt_native_fnentry* entry = *entryPtr;
      klt_hash_map_put(interpreter->nativeFunctions, entry->name, &entry->fn);
    }
  }

  interpreter->env = klt_environment_make(NULL);

  return interpreter;
}

void klt_interpreter_delete(klt_interpreter* self) {
  klt_hash_map_delete(self->functions);
  klt_hash_map_delete(self->nativeFunctions);
  klt_environment_destroy(self->env);
  free(self);
}

klt_interpreter_result klt_interpreter_run(klt_interpreter* self) {
  assert(self);

  klt_node** mainPtr = (klt_node**)klt_hash_map_get(self->functions, "main");
  if (mainPtr == NULL) {
    klt_error_fatal("Your program needs a main function!");
  }
  klt_node* main = *mainPtr;

  if (main->function_n.returnType == NULL ||
      !klt_str_equals(main->function_n.returnType, "bool")) {
    klt_error_fatal("Main function should return bool.");
  }

  return klt_interpreter_run_fn(self, main, NULL);
}

klt_interpreter_result klt_interpreter_run_fn(klt_interpreter* self,
                                              klt_node* func,
                                              klt_node_fnparam_vector* params) {
  assert(self);
  assert(func);
  assert(func->type == NODE_FUNCTION);

  klt_environment* old = self->env;
  self->env = klt_environment_make(NULL);

  if (params != NULL && func->function_n.params != NULL) {
    for (int i = 0; i < params->size; i++) {
      klt_node_fnparam* param = *(klt_node_fnparam**)klt_vector_get(params, i);
      klt_node_fnparam* fnParam =
          *(klt_node_fnparam**)klt_vector_get(func->function_n.params, i);

      klt_node_valuetype actualType = param->type;
      void* actualValue = param->value;

      if (param->type == NODE_VALUE_TYPE_VAR) {
        klt_node* real_var =
            klt_environment_get(old, (klt_str)param->value);
        if (real_var == NULL) {
          klt_error_fatal("Variable not defined: %s", (klt_str)param->value);
        }
        actualType = klt_parser_str_to_nodevaluetype(real_var->vardec_n.varType);
        actualValue = real_var->vardec_n.varValue;
      }

      if (fnParam->type != NODE_VALUE_TYPE_ANY && fnParam->type != actualType) {
        klt_error_fatal(
            "Argument %d to function '%s' expected type '%s', but got '%s'",
            i + 1, func->function_n.name,
            klt_parser_nodevaluetype_to_str(fnParam->type),
            klt_parser_nodevaluetype_to_str(actualType));
      }

      klt_node* var = klt_var_dec_node_make(
          fnParam->value, klt_parser_nodevaluetype_to_str(fnParam->type),
          actualType, actualValue);
      klt_environment_define(self->env, var->vardec_n.varName, var);
    }
  }

  for (int i = 0; i < func->function_n.body->size; i++) {
    klt_node** stmtPtr = (klt_node**)klt_vector_get(func->function_n.body, i);
    if (stmtPtr != NULL) {
      klt_node* stmt = *stmtPtr;
      klt_interpreter_result result = klt_interpreter_run_node(self, stmt);
      if (result.type == IRT_RETURN) {
        klt_environment* to_destroy = self->env;
        self->env = old;
        klt_environment_destroy(to_destroy);
        return result;
      }
    }
  }

  klt_environment* to_destroy = self->env;
  self->env = old;
  klt_environment_destroy(to_destroy);

  // default value
  return (klt_interpreter_result){.type = IRT_FUNC, .data = NULL};
}

klt_interpreter_result klt_interpreter_run_node(klt_interpreter* self,
                                                klt_node* node) {
  assert(self);
  assert(node);

  switch (node->type) {
    case NODE_CALL: {
      klt_node** calledPtr = (klt_node**)klt_hash_map_get(
          self->functions, node->call_n.functionName);
      klt_native_fn* nativeFnPtr = (klt_native_fn*)klt_hash_map_get(
          self->nativeFunctions, node->call_n.functionName);

      if (calledPtr != NULL) {
        klt_node* called = *calledPtr;
        klt_interpreter_result result =
            klt_interpreter_run_fn(self, called, node->call_n.functionParams);
        return result;
      } else if (nativeFnPtr != NULL) {
        klt_native_fndata* nativeFnData = malloc(sizeof(klt_native_fndata));
        nativeFnData->params = node->call_n.functionParams;
        nativeFnData->env = self->env;

        klt_native_fn nativeFn = *nativeFnPtr;
        klt_node* nativeFnResult = nativeFn(nativeFnData);
        klt_interpreter_result result =
            (klt_interpreter_result){.data = nativeFnResult, .type = IRT_FUNC};
        return result;
      } else {
        klt_error_fatal("Function not found: %s", node->call_n.functionName);
      }
    }

    case NODE_RETURN: {
      void* value = NULL;
      if (node->return_n.returnValue != NULL) {
        value = node->return_n.returnValue;  // or evaluate this node if needed
      }
      return (klt_interpreter_result){.type = IRT_RETURN, .data = value};
    }

    case NODE_VARDEC: {
      /*printf("[DEBUG] Declaring variable '%s' of type '%s' with value '%s'\n",
             node->vardec_n.varName, node->vardec_n.varType,
             (klt_str)node->vardec_n.varValue);*/
      klt_environment_define(self->env, node->vardec_n.varName, node);
      return (klt_interpreter_result){.type = IRT_FUNC, .data = NULL};
    }

    default:
      klt_error_fatal("Unknown node type %d", node->type);
  }
  return (klt_interpreter_result){.type = IRT_FUNC, .data = NULL};
}