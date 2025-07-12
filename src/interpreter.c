#include "kilate/interpreter.h"

#include <stdio.h>
#include <string.h>

#include "kilate/environment.h"
#include "kilate/error.h"
#include "kilate/file.h"
#include "kilate/hashmap.h"
#include "kilate/node.h"
#include "kilate/parser.h"

klt_interpreter* klt_interpreter_make(
    klt_node_vector* nodes_vector,
    klt_node_vector* native_functions_nodes_vector) {
  if (nodes_vector == NULL)
    klt_error_fatal("Node's Vector is invalid.");
  if (native_functions_nodes_vector == NULL)
    klt_error_fatal("Native Functions Node's Vector is invalid.");

  klt_interpreter* interpreter = malloc(sizeof(klt_interpreter));
  interpreter->functions = klt_hash_map_make(sizeof(klt_node*));
  interpreter->native_functions = klt_hash_map_make(sizeof(klt_native_fn));

  // register all funcs
  for (size_t i = 0; i < nodes_vector->size; i++) {
    klt_node** nodePtr = (klt_node**)klt_vector_get(nodes_vector, i);
    if (nodePtr != NULL) {
      klt_node* node = *nodePtr;
      if (node->type == NODE_FUNCTION) {
        klt_hash_map_put(interpreter->functions, node->function_n.fn_name,
                         nodePtr);
      }
    }
  }

  for (size_t i = 0; i < native_functions_nodes_vector->size; i++) {
    klt_native_fnentry** entryPtr =
        (klt_native_fnentry**)klt_vector_get(native_functions_nodes_vector, i);
    if (entryPtr != NULL) {
      klt_native_fnentry* entry = *entryPtr;
      klt_hash_map_put(interpreter->native_functions, entry->name, entryPtr);
    }
  }

  interpreter->env = klt_environment_make(NULL);

  return interpreter;
}

void klt_interpreter_delete(klt_interpreter* self) {
  if (self == NULL)
    return;
  klt_hash_map_delete(self->functions);
  klt_hash_map_delete(self->native_functions);
  klt_environment_destroy(self->env);
  free(self);
}

klt_interpreter_result klt_interpreter_run(klt_interpreter* self) {
  if (self == NULL)
    klt_error_fatal("Interpreter is invalid.");

  klt_node** mainPtr = (klt_node**)klt_hash_map_get(self->functions, "main");
  if (mainPtr == NULL) {
    klt_error_fatal("Your program needs a main function!");
  }
  klt_node* main = *mainPtr;

  if (main->function_n.fn_return_type == NULL ||
      !klt_str_equals(main->function_n.fn_return_type, "bool")) {
    klt_error_fatal("Main function should return bool.");
  }

  return klt_interpreter_run_fn(self, main, NULL);
}

klt_interpreter_result klt_interpreter_run_fn(klt_interpreter* self,
                                              klt_node* func,
                                              klt_node_fnparam_vector* params) {
  if (self == NULL)
    klt_error_fatal("Interpreter is invalid.");

  if (func == NULL || func->type != NODE_FUNCTION) {
    klt_error_fatal("Functin Node Not is a Valid Function");
  }

  klt_environment* old = self->env;
  self->env = klt_environment_make(NULL);

  if (params != NULL && func->function_n.fn_params != NULL) {
    for (size_t i = 0; i < params->size; i++) {
      klt_node_fnparam* param = *(klt_node_fnparam**)klt_vector_get(params, i);
      klt_node_fnparam* fnParam =
          *(klt_node_fnparam**)klt_vector_get(func->function_n.fn_params, i);

      klt_node_valuetype actualType = param->type;
      void* actualValue = param->value;

      if (param->type == NODE_VALUE_TYPE_VAR) {
        klt_node* real_var = klt_environment_getvar(old, (klt_str)param->value);
        if (real_var == NULL) {
          klt_error_fatal("Variable not defined: %s", (klt_str)param->value);
        }
        actualType =
            klt_parser_str_to_nodevaluetype(real_var->vardec_n.var_type);
        actualValue = real_var->vardec_n.var_value;
      }

      if (fnParam->type != NODE_VALUE_TYPE_ANY && fnParam->type != actualType) {
        klt_error_fatal(
            "Argument %zu to function '%s' expected type '%s', but got '%s'",
            i + 1, func->function_n.fn_name,
            klt_parser_nodevaluetype_to_str(fnParam->type),
            klt_parser_nodevaluetype_to_str(actualType));
      }

      klt_node* var = klt_var_dec_node_make(
          fnParam->value, klt_parser_nodevaluetype_to_str(fnParam->type),
          actualType, actualValue);
      klt_node* var_copy = klt_node_copy(var);
      klt_environment_definevar(self->env, var_copy->vardec_n.var_name, var_copy);
    }
  }

  for (size_t i = 0; i < func->function_n.fn_body->size; i++) {
    klt_node** stmtPtr =
        (klt_node**)klt_vector_get(func->function_n.fn_body, i);
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
  if (self == NULL)
    klt_error_fatal("Interpreter is invalid.");
  if (node == NULL)
    klt_error_fatal("Node is invalid.");

  switch (node->type) {
    case NODE_CALL: {
      klt_node** calledPtr = (klt_node**)klt_hash_map_get(
          self->functions, node->call_n.fn_call_name);
      klt_native_fnentry** nativeFnEntryPtr =
          (klt_native_fnentry**)klt_hash_map_get(self->native_functions,
                                                 node->call_n.fn_call_name);

      // if ptr not null, so its a fn
      if (calledPtr != NULL) {
        klt_node* called = *calledPtr;
        klt_interpreter_result result =
            klt_interpreter_run_fn(self, called, node->call_n.fn_call_params);
        return result;
      } else if (nativeFnEntryPtr != NULL) {
        // else if native ptr is not null, so its native fn
        klt_native_fndata* nativeFnData = malloc(sizeof(klt_native_fndata));
        nativeFnData->params = node->call_n.fn_call_params;
        nativeFnData->env = self->env;

        klt_native_fnentry* nativeFnEntry = *nativeFnEntryPtr;
        klt_native_fn nativeFn = *nativeFnEntry->fn;
        klt_node* nativeFnResult = nativeFn(nativeFnData);
        klt_interpreter_result result =
            (klt_interpreter_result){.data = nativeFnResult, .type = IRT_FUNC};
        return result;
      } else {
        // else not found
        klt_error_fatal("Function not found: %s", node->call_n.fn_call_name);
      }
    }

    case NODE_RETURN: {
      void* value = NULL;
      if (node->return_n.return_value != NULL) {
        value = node->return_n.return_value;  // or evaluate this node if needed
      }
      return (klt_interpreter_result){.type = IRT_RETURN, .data = value};
    }

    case NODE_VARDEC: {
      klt_environment_definevar(self->env, node->vardec_n.var_name, klt_node_copy(node));
      return (klt_interpreter_result){.type = IRT_FUNC, .data = NULL};
    }

    default:
      klt_error_fatal("Unknown node type %d", node->type);
  }
  return (klt_interpreter_result){.type = IRT_FUNC, .data = NULL};
}