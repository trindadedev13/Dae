#include "kilate/util/native.h"

#include <stdlib.h>
#include <string.h>

#include "kilate/native.h"

klt_str klt_native_fndata_getstr(klt_native_fndata* data, size_t index) {
  if (index >= data->params->size)
    return "";

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_STRING) {
    return param->value;
  }

  klt_node* var = klt_environment_getvar(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_STRING)
    return NULL;

  if (var->vardec_n.var_value == NULL)
    return "";

  return (klt_str)var->vardec_n.var_value;
}

int klt_native_fndata_getint(klt_native_fndata* data, size_t index, klt_bool* ok) {
  if (index >= data->params->size) {
    if (ok) *ok = false;
    return 0;
  }

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_INT) {
    if (ok) *ok = true;
    return (int)(intptr_t)param->value;
  }

  klt_node* var = klt_environment_getvar(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_INT) {
    if (ok) *ok = false;
    return 0;
  }

  if (ok) *ok = true;
  return (int)(intptr_t)var->vardec_n.var_value;
}

float klt_native_fndata_getfloat(klt_native_fndata* data, size_t index, klt_bool* ok) {
  if (index >= data->params->size) {
    if (ok) *ok = false;
    return 0;
  }

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_FLOAT) {
    if (ok) *ok = true;
    return (float)(intptr_t)param->value;
  }

  klt_node* var = klt_environment_getvar(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_FLOAT) {
    if (ok) *ok = false;
    return 0;
  }

  if (ok) *ok = true;
  return *(float*)(intptr_t)var->vardec_n.var_value;
}

long klt_native_fndata_getlong(klt_native_fndata* data, size_t index, klt_bool* ok) {
  if (index >= data->params->size) {
    if (ok) *ok = false;
    return 0;
  }

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_LONG) {
    if (ok) *ok = true;
    return (long)(intptr_t)param->value;
  }

  klt_node* var = klt_environment_getvar(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_LONG) {
    if (ok) *ok = false;
    return 0;
  }

  if (ok) *ok = true;
  return (long)(intptr_t)var->vardec_n.var_value;
}

klt_bool klt_native_fndata_getklt_bool(klt_native_fndata* data, size_t index, klt_bool* ok) {
  if (index >= data->params->size) {
    if (ok) *ok = false;
    return false;
  }

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_BOOL) {
    if (ok) *ok = true;
    return (klt_bool)(intptr_t)param->value;
  }

  klt_node* var = klt_environment_getvar(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_BOOL) {
    if (ok) *ok = false;
    return false;
  }

  if (ok) *ok = true;
  return (klt_bool)(intptr_t)var->vardec_n.var_value;
}

void klt_params_add(klt_str_vector* params, const klt_str param) {
  klt_str str = (klt_str)param;
  klt_vector_push_back(params, &str);
}

klt_native_fnentry* klt_native_fnentry_make(const klt_str name,
                                  klt_str_vector* reqParams,
                                  klt_native_fn fn) {
  klt_native_fnentry* entry =
      (klt_native_fnentry*)malloc(sizeof(klt_native_fnentry));
  entry->name = strdup(name);
  entry->requiredParams = reqParams;
  entry->fn = fn;
  return entry;
}