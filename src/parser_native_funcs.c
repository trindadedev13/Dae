#include "kilate/parser_native_funcs.h"

#include <stdio.h>

#include "kilate/lexer.h"
#include "kilate/node.h"
#include "kilate/parser.h"
#include "kilate/string.h"
#include "kilate/vector.h"

#include <stdlib.h>
#include <string.h>

String Parser_Native_Internal_InterpretEscapes(String input) {
  size_t len = String_Length(input);
  String output = malloc(len + 1);
  size_t j = 0;

  for (size_t i = 0; i < len; ++i) {
    if (input[i] == '\\' && i + 1 < len) {
      i++;
      switch (input[i]) {
        case 'n':
          output[j++] = '\n';
          break;
        case 't':
          output[j++] = '\t';
          break;
        case '\\':
          output[j++] = '\\';
          break;
        default:
          output[j++] = input[i];
          break;
      }
    } else {
      output[j++] = input[i];
    }
  }

  output[j] = '\0';
  return output;
}

Node* Parser_Native_Print(NativeFnData* data) {
  for (size_t i = 0; i < data->params->size; ++i) {
    NodeFunctionParam* param =
        *(NodeFunctionParam**)Vector_Get(data->params, i);
    if (param->type == NODE_VALUE_TYPE_VAR) {
      Node* var = ScopeStack_Get(data->varStack, param->value);
      void* value = var->vardec_n.varValue;
      switch (var->vardec_n.varValueType) {
        case NODE_VALUE_TYPE_NUMBER: {
          printf("%d", (int)(intptr_t)value);
          break;
        }
        case NODE_VALUE_TYPE_STRING:
          printf("%s", (String)value);
          break;
        case NODE_VALUE_TYPE_BOOL:
          printf("%s", (bool)(intptr_t)value ? "true" : "false");
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
      free(data);
      return NULL;
    }
    String interpreted = Parser_Native_Internal_InterpretEscapes(param->value);
    printf("%s", interpreted);
    free(interpreted);
  }
  free(data);
  return NULL;
}

Node* Parser_Native_System(NativeFnData* data) {
  for (size_t i = 0; i < data->params->size; ++i) {
    NodeFunctionParam* param =
        *(NodeFunctionParam**)Vector_Get(data->params, i);
    if (param->type == NODE_VALUE_TYPE_VAR) {
      Node* var = ScopeStack_Get(data->varStack, param->value);
      void* value = var->vardec_n.varValue;
      switch (var->vardec_n.varValueType) {
        case NODE_VALUE_TYPE_STRING:
          system((String)value);
          break;
        default:
          break;
      }
      free(data);
      return NULL;
    }
    system(param->value);
  }
  free(data);
  return NULL;
}