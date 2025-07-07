#include "kilate/node.h"

#include <stdlib.h>
#include <string.h>

#include "kilate/string.h"
#include "kilate/vector.h"

void klt_node_delete(klt_node* node) {
  if (node == NULL) return;
  if (node->type == NODE_FUNCTION) {
    free(node->function_n.fn_name);
    if (node->function_n.fn_return_type != NULL) {
      free(node->function_n.fn_return_type);
    }
    // free body nodes
    for (size_t j = 0; j < node->function_n.fn_body->size; ++j) {
      klt_node** body_nodePtr =
          (klt_node**)klt_vector_get(node->function_n.fn_body, j);
      if (body_nodePtr != NULL) {
        klt_node* body_node = *body_nodePtr;
        klt_node_delete(body_node);
      }
    }
    klt_vector_delete(node->function_n.fn_body);
    // free param nodes
    for (size_t j = 0; j < node->function_n.fn_params->size; ++j) {
      klt_node_fnparam* param =
          *(klt_node_fnparam**)klt_vector_get(node->function_n.fn_params, j);
      free(param->value);
      free(param);
    }
    klt_vector_delete(node->function_n.fn_params);
  } else if (node->type == NODE_IMPORT) {
    free(node->import_n.import_path);
  } else if (node->type == NODE_CALL) {
    free(node->call_n.fn_call_name);
    klt_node_delete_params(node->call_n.fn_call_params);
  } else if (node->type == NODE_VARDEC) {
    free(node->vardec_n.var_name);
    free(node->vardec_n.var_type);
  }
  free(node);
}

klt_node* klt_node_copy(klt_node* node) {
  if (!node)
    return NULL;

  klt_node* new = malloc(sizeof(klt_node));
  if (!new)
    return NULL;

  new->type = node->type;

  if (node->type == NODE_FUNCTION) {
    new->function_n.fn_name = NULL;
    new->function_n.fn_return_type = NULL;
    if (node->function_n.fn_name != NULL) {
      new->function_n.fn_name = strdup(node->function_n.fn_name);
    }
    if (node->function_n.fn_return_type != NULL) {
      new->function_n.fn_return_type = strdup(node->function_n.fn_return_type);
    }

    new->function_n.fn_body = klt_vector_make(sizeof(klt_node*));
    for (size_t i = 0; i < node->function_n.fn_body->size; ++i) {
      klt_node* child_node =
          *(klt_node**)klt_vector_get(node->function_n.fn_body, i);
      klt_node* child_copy = klt_node_copy(child_node);
      klt_vector_push_back(new->function_n.fn_body, &child_copy);
    }

    new->function_n.fn_params = klt_vector_make(sizeof(klt_node_fnparam*));
    for (size_t i = 0; i < node->function_n.fn_params->size; ++i) {
      klt_node_fnparam* param =
          *(klt_node_fnparam**)klt_vector_get(node->function_n.fn_params, i);
      klt_node_fnparam* param_copy = klt_node_fnparam_copy(param);
      klt_vector_push_back(new->function_n.fn_params, &param_copy);
    }
  } else if (node->type == NODE_CALL) {
    new->call_n.fn_call_name = NULL;
    if (node->call_n.fn_call_name != NULL) {
      new->call_n.fn_call_name = strdup(node->call_n.fn_call_name);
    }
    new->call_n.fn_call_params = klt_vector_make(sizeof(klt_node_fnparam*));
    for (size_t i = 0; i < node->call_n.fn_call_params->size; ++i) {
      klt_node_fnparam* param =
          *(klt_node_fnparam**)klt_vector_get(node->call_n.fn_call_params, i);
      klt_node_fnparam* param_copy = klt_node_fnparam_copy(param);
      klt_vector_push_back(new->call_n.fn_call_params, &param_copy);
    }
  } else if (node->type == NODE_RETURN) {
    new->return_n.return_type = node->return_n.return_type;
    new->return_n.return_value = node->return_n.return_value;  // void*
  } else if (node->type == NODE_VARDEC) {
    new->vardec_n.var_name = NULL;
    new->vardec_n.var_type = NULL;
    if (node->vardec_n.var_name != NULL) {
      new->vardec_n.var_name = strdup(node->vardec_n.var_name);
    }
    if (node->vardec_n.var_type != NULL) {
      new->vardec_n.var_type = strdup(node->vardec_n.var_type);
    }
    new->vardec_n.var_value_type = node->vardec_n.var_value_type;
    new->vardec_n.var_value = node->vardec_n.var_value;  // void*
  } else if (node->type == NODE_IMPORT) {
    new->import_n.import_path = strdup(node->import_n.import_path);
  }
  return new;
}

klt_node_fnparam* klt_node_fnparam_copy(klt_node_fnparam* param) {
  klt_node_fnparam* new = malloc(sizeof(klt_node_fnparam));
  new->value = strdup(param->value);
  new->type = param->type;
  return new;
}

void klt_node_delete_params(klt_node_fnparam_vector* params) {
  if (params == NULL)
    return;
  for (size_t i = 0; i < params->size; ++i) {
    klt_node_fnparam* param = *(klt_node_fnparam**)klt_vector_get(params, i);
    free(param->value);
    free(param);
  }
  klt_vector_delete(params);
}

klt_node* klt_function_node_make(klt_str name,
                                 klt_str return_type,
                                 klt_node_vector* body,
                                 klt_node_fnparam_vector* params) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_FUNCTION;
  node->function_n.fn_name = strdup(name);
  if (return_type != NULL) {
    node->function_n.fn_return_type = strdup(return_type);
  } else {
    node->function_n.fn_return_type = NULL;
  }
  node->function_n.fn_body = body;
  node->function_n.fn_params = params;
  return node;
}

klt_node* klt_call_node_make(const klt_str functionName,
                             klt_node_fnparam_vector* functionParams) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_CALL;
  node->call_n.fn_call_name = strdup(functionName);
  node->call_n.fn_call_params = functionParams;
  return node;
}

klt_node* klt_return_node_make(klt_node_valuetype return_type,
                               void* return_value) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_RETURN;
  node->return_n.return_type = return_type;
  node->return_n.return_value = return_value;
  return node;
}

klt_node* klt_var_dec_node_make(klt_str name,
                                klt_str type,
                                klt_node_valuetype valueType,
                                void* value) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_VARDEC;
  node->vardec_n.var_name = strdup(name);
  node->vardec_n.var_type = strdup(type);
  node->vardec_n.var_value_type = valueType;
  node->vardec_n.var_value = value;
  return node;
}

klt_node* klt_import_node_make(klt_str path) {
  klt_node* node = malloc(sizeof(klt_node));
  node->type = NODE_IMPORT;
  node->import_n.import_path = strdup(path);
  return node;
}