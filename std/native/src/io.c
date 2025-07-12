#include "io.h"

#include <kilate/native.h>
#include <kilate/util/native.h>
#include <kilate/node.h>

#include "sys.h"

klt_node* std_print(klt_native_fndata* data) {
  for (size_t i = 0; i < data->params->size; ++i) {
    klt_node_fnparam* param =
        *(klt_node_fnparam**)klt_vector_get(data->params, i);
    if (param->type == NODE_VALUE_TYPE_VAR) {
      klt_node* var = klt_environment_getvar(data->env, param->value);
      void* value = var->vardec_n.var_value;
      switch (var->vardec_n.var_value_type) {
        case NODE_VALUE_TYPE_INT: {
          printf("%d", (int)(intptr_t)value);
          break;
        }
        case NODE_VALUE_TYPE_FLOAT: {
          printf("%f", *(float*)value);
          break;
        }
        case NODE_VALUE_TYPE_LONG: {
          printf("%ld", (long)(intptr_t)value);
          break;
        }
        case NODE_VALUE_TYPE_STRING:
          printf("%s", (klt_str)value);
          break;
        case NODE_VALUE_TYPE_BOOL:
          printf("%s", (klt_bool)(intptr_t)value ? "true" : "false");
          break;
        case NODE_VALUE_TYPE_FUNC:
          // Does nothing for now
          break;
        case NODE_VALUE_TYPE_VAR:
          // Does nothing for now
          break;
        default:
          // Does nothing for now
          break;
      }
      continue;
    }
    printf("%s", param->value);
  }
  free(data);
  return NULL;
}

klt_node* std_system(klt_native_fndata* data) {
  klt_str cmd = klt_native_fndata_getstr(data, 0);
  if (cmd != NULL)
    system(cmd);
  return NULL;
}

klt_node* std_sleep(klt_native_fndata* data) {
  klt_bool ok;
  long time = klt_native_fndata_getlong(data, 0, &ok);
  if (ok)
    sys_sleep(time);
  return NULL;
}