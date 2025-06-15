#include "dae/parser_native_funcs.h"

#include <stdio.h>

#include "dae/lexer.h"
#include "dae/node.h"
#include "dae/parser.h"
#include "dae/string.h"
#include "dae/vector.h"

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

Node* Parser_Native_Print(StringVector* args) {
  for (size_t i = 0; i < args->size; ++i) {
    String arg = *(String*)Vector_Get(args, i);
    String interpreted = Parser_Native_Internal_InterpretEscapes(arg);
    printf("%s", interpreted);
    free(interpreted);
  }
  Parser_DeleteArgs(args);
  return NULL;
}