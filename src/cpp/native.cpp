#ifdef __cplusplus

#include "kilate/cpp/native.hpp"

#include <cstring>
#include <iostream>
#include <string>

extern "C" {
#include "kilate/native.h"
}

namespace kilate {
namespace native {
std::string getFnDataString(klt_native_fndata* data, size_t index) {
  if (index >= data->params->size)
    return "";

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_STRING) {
    return param->value;
  }

  klt_node* var = klt_environment_get(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_STRING)
    return nullptr;

  if (var->vardec_n.var_value == nullptr)
    return "";

  return std::string{(char*)var->vardec_n.var_value};
}

int getFnDataInt(klt_native_fndata* data, size_t index, bool* ok) {
  if (index >= data->params->size) {
    if (ok)
      *ok = false;
    return 0;
  }

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_INT) {
    if (ok)
      *ok = true;
    return (int)(intptr_t)param->value;
  }

  klt_node* var = klt_environment_get(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_INT) {
    if (ok)
      *ok = false;
    return 0;
  }

  if (ok)
    *ok = true;
  return (int)(intptr_t)var->vardec_n.var_value;
}

float getFnDataFloat(klt_native_fndata* data, size_t index, bool* ok) {
  if (index >= data->params->size) {
    if (ok)
      *ok = false;
    return 0;
  }

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_FLOAT) {
    if (ok)
      *ok = true;
    return (int)(intptr_t)param->value;
  }

  klt_node* var = klt_environment_get(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_FLOAT) {
    if (ok)
      *ok = false;
    return 0;
  }

  if (ok)
    *ok = true;
  return *(float*)(intptr_t)var->vardec_n.var_value;
}

long getFnDataLong(klt_native_fndata* data, size_t index, bool* ok) {
  if (index >= data->params->size) {
    if (ok)
      *ok = false;
    return 0;
  }

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_LONG) {
    if (ok)
      *ok = true;
    return (int)(intptr_t)param->value;
  }

  klt_node* var = klt_environment_get(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_LONG) {
    if (ok)
      *ok = false;
    return 0;
  }

  if (ok)
    *ok = true;
  return (long)(intptr_t)var->vardec_n.var_value;
}

bool getFnDataBool(klt_native_fndata* data, size_t index, bool* ok) {
  if (index >= data->params->size) {
    if (ok)
      *ok = false;
    return false;
  }

  klt_node_fnparam* param =
      *(klt_node_fnparam**)klt_vector_get(data->params, index);
  if (param->type == NODE_VALUE_TYPE_BOOL) {
    if (ok)
      *ok = true;
    return (bool)(intptr_t)param->value;
  }

  klt_node* var = klt_environment_get(data->env, param->value);
  if (!var || var->vardec_n.var_value_type != NODE_VALUE_TYPE_BOOL) {
    if (ok)
      *ok = false;
    return false;
  }

  if (ok)
    *ok = true;
  return (bool)(intptr_t)var->vardec_n.var_value;
}

void addParam(klt_str_vector* params, const std::string& param) {
  klt_str str = const_cast<char*>(param.c_str());
  klt_vector_push_back(params, &str);
}

klt_native_fnentry* createFnEntry(const std::string& name,
                                  klt_str_vector* reqParams,
                                  klt_native_fn fn) {
  klt_native_fnentry* entry =
      (klt_native_fnentry*)std::malloc(sizeof(klt_native_fnentry));
  entry->name = strdup(name.c_str());
  entry->requiredParams = reqParams;
  entry->fn = fn;
  return entry;
}
}  // namespace native
namespace node {
klt_node* createReturnNode(klt_node_valuetype type, void* value) {
  return klt_return_node_make(type, value);
}
}  // namespace node
}  // namespace kilate

#endif