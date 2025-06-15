#include "dae/parser.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

#include "dae/error.h"
#include "dae/lexer.h"
#include "dae/node.h"
#include "dae/parser_native_funcs.h"
#include "dae/string.h"
#include "dae/vector.h"

Parser* Parser_New(TokenVector* tokens) {
  Parser* parser = malloc(sizeof(Parser));
  parser->tokens = tokens;
  parser->functions = Vector_New(sizeof(Node*));
  parser->nativeFunctions = Vector_New(sizeof(NativeFunctionEntry*));
  parser->__pos__ = 0;
  return parser;
}

void Parser_Delete(Parser* parser) {
  for (size_t i = 0; i < parser->functions->size; ++i) {
    Node* node = *(Node**)Vector_Get(parser->functions, i);
    // free body nodes if its function node
    if (node->type == NODE_FUNCTION) {
      NodeVector* body = node->function_n.body;
      for (size_t j = 0; j < body->size; ++j) {
        Node* bodyNode = *(Node**)Vector_Get(body, j);
        free(bodyNode);
      }
      NodeFuncParamVector* params = node->function_n.params;
      for (size_t j = 0; j < params->size; ++j) {
        NodeFunctionParam* param = *(NodeFunctionParam**)Vector_Get(params, j);
        free(param);
      }
      Vector_Delete(node->function_n.body);
      Vector_Delete(node->function_n.params);
    }
    free(node);
  }
  for (size_t i = 0; i < parser->nativeFunctions->size; ++i) {
    NativeFunctionEntry* entry =
        *(NativeFunctionEntry**)Vector_Get(parser->nativeFunctions, i);
    free(entry->name);
    free(entry);
  }
  Vector_Delete(parser->functions);
  Vector_Delete(parser->nativeFunctions);
  free(parser);
}

void Parser_DeleteArgs(StringVector* args) {
  for (size_t i = 0; i < args->size; ++i) {
    String arg = *(String*)Vector_Get(args, i);
    free(arg);
  }
  Vector_Delete(args);
}

Token* Parser_Consume(Parser* parser, TokenType exType) {
  Token* token = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
  if (token->type != exType) {
    Parser_Error("Expected %s, but got %s", Token_GetTypeName(exType),
                 Token_GetTypeName(token->type));
    return NULL;
  }
  parser->__pos__++;
  return token;
}

Node* Parser_FindFunction(Parser* parser, String name) {
  for (size_t i = 0; i < parser->functions->size; i++) {
    Node* fn = *(Node**)Vector_Get(parser->functions, i);
    if (String_Equals(fn->function_n.name, name)) {
      return fn;
    }
  }
  return NULL;
}

void Parser_RegisterNativeFunction(Parser* parser,
                                   String name,
                                   NativeFunction fn) {
  NativeFunctionEntry* entry = malloc(sizeof(NativeFunctionEntry));
  entry->name = strdup(name);
  entry->fn = fn;
  Vector_PushBack(parser->nativeFunctions, &entry);
}

NativeFunction Parser_FindNativeFunction(Parser* parser, String name) {
  for (size_t i = 0; i < parser->nativeFunctions->size; ++i) {
    NativeFunctionEntry* entry =
        *(NativeFunctionEntry**)Vector_Get(parser->nativeFunctions, i);
    if (String_Equals(entry->name, name)) {
      return entry->fn;
    }
  }
  return NULL;
}

void Parser_RegisterNatives(Parser* parser) {
  Parser_RegisterNativeFunction(parser, "print", Parser_Native_Print);
}

String Parser_GetTypeFromToken(TokenType type) {
  switch (type) {
    case TOKEN_STRING:
      return "string";
    case TOKEN_BOOLEAN:
      return "bool";
    case TOKEN_NUMBER:
      return "number";
    default:
      return "unknownType";
  }
}

Node* Parser_ParseStatement(Parser* parser) {
  Token* token = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
  if (String_Equals(token->text, "return")) {
    Parser_Consume(parser, TOKEN_KEYWORD);
    Parser_Consume(parser, TOKEN_ARROW);
    Token* next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
    void* value;
    NodeReturnType type;
    if (next->type == TOKEN_BOOLEAN) {
      bool rawbool = false;
      String boolStr = Parser_Consume(parser, TOKEN_BOOLEAN)->text;
      if (String_Equals(boolStr, "true"))
        rawbool = true;
      value = (void*)(intptr_t)rawbool;
      type = NODE_RETURN_BOOL;
    } else if (next->type == TOKEN_NUMBER) {
      value = (void*)(intptr_t)String_ToInt(
          Parser_Consume(parser, TOKEN_NUMBER)->text);
      type = NODE_RETURN_NUMBER;
    } else if (next->type == TOKEN_STRING) {
      value = Parser_Consume(parser, TOKEN_STRING)->text;
      type = NODE_RETURN_STRING;
    }
    return ReturnNode_New(type, value);
  } else if (token->type == TOKEN_IDENTIFIER) {
    String name = Parser_Consume(parser, TOKEN_IDENTIFIER)->text;
    Token* next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);

    // call with () no args
    if (next->type == TOKEN_LPAREN) {
      Parser_Consume(parser, TOKEN_LPAREN);
      Parser_Consume(parser, TOKEN_RPAREN);
      return CallNode_New(name, NULL);
    }

    // call with ->
    if (next->type == TOKEN_ARROW) {
      Parser_Consume(parser, TOKEN_ARROW);
      StringVector* args = Vector_New(sizeof(String));

      while (true) {
        Token* arg = *(Token**)Vector_Get(parser->tokens, parser->__pos__);

        if (arg->type != TOKEN_STRING && arg->type != TOKEN_BOOLEAN &&
            arg->type != TOKEN_NUMBER) {
          break;
        }

        String argText = strdup(arg->text);
        Vector_PushBack(args, &argText);
        Parser_Consume(parser, arg->type);

        Token* comma = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
        if (comma->type == TOKEN_COMMA) {
          Parser_Consume(parser, TOKEN_COMMA);
        } else {
          break;
        }
      }

      Node* fn = Parser_FindFunction(parser, name);
      if (fn != NULL) {
        size_t expected = fn->function_n.params->size;
        if (args->size != expected) {
          Parser_Error("Function '%s' expects %zu arguments but got %zu.", name,
                       expected, args->size);
        }

        for (size_t i = 0; i < expected; ++i) {
          NodeFunctionParam* param =
              *(NodeFunctionParam**)Vector_Get(fn->function_n.params, i);
          Token* token = *(Token**)Vector_Get(parser->tokens,
                                              parser->__pos__ - args->size + i);
          String expectedType = param->type;
          String actualType = Parser_GetTypeFromToken(token->type);

          if (!String_Equals(expectedType, actualType)) {
            Parser_Error("Argument %zu to '%s' must be of type %s, got %s",
                         i + 1, name, expectedType, actualType);
          }
        }

        return CallNode_New(name, args);
      }

      // check if its native
      NativeFunction nativeFn = Parser_FindNativeFunction(parser, name);
      if (nativeFn != NULL) {
        return CallNode_New(name, args);
      }

      Parser_Error(
          "Function '%s' is not declared and is not a native function.", name);
    }

    Parser_Error("Unexpected token after identifier: %s", next->text);
  }
  Parser_Error("Unknown statement: %s", token->text);
  return NULL;
}

Node* Parser_ParseFunction(Parser* parser) {
  Parser_Consume(parser, TOKEN_KEYWORD);
  String name = Parser_Consume(parser, TOKEN_IDENTIFIER)->text;

  Parser_Consume(parser, TOKEN_LPAREN);
  NodeFuncParamVector* params = Vector_New(sizeof(NodeFunctionParam*));

  while (true) {
    Token* next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
    if (next->type == TOKEN_RPAREN) {
      Parser_Consume(parser, TOKEN_RPAREN);
      break;
    }

    String type = Parser_Consume(parser, TOKEN_TYPE)->text;
    Parser_Consume(parser, TOKEN_COLON);
    String name = Parser_Consume(parser, TOKEN_IDENTIFIER)->text;

    NodeFunctionParam* param = malloc(sizeof(NodeFunctionParam));
    param->name = name;
    param->type = type;
    Vector_PushBack(params, &param);

    next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
    if (next->type == TOKEN_COMMA) {
      Parser_Consume(parser, TOKEN_COMMA);
    }
  }

  String returnType = NULL;
  if ((*(Token**)Vector_Get(parser->tokens, parser->__pos__))->type ==
      TOKEN_COLON) {
    Parser_Consume(parser, TOKEN_COLON);
    returnType = Parser_Consume(parser, TOKEN_TYPE)->text;
  }

  Parser_Consume(parser, TOKEN_LBRACE);
  NodeVector* body = Vector_New(sizeof(Node*));
  while ((*(Token**)Vector_Get(parser->tokens, parser->__pos__))->type !=
         TOKEN_RBRACE) {
    Node* node = Parser_ParseStatement(parser);
    Vector_PushBack(body, &node);
  }

  Parser_Consume(parser, TOKEN_RBRACE);
  return FunctionNode_New(name, returnType, body, params);
}

void Parser_ParseProgram(Parser* parser) {
  Parser_RegisterNatives(parser);
  do {
    Node* node = Parser_ParseFunction(parser);
    Vector_PushBack(parser->functions, &node);
  } while ((*(Token**)Vector_Get(parser->tokens, parser->__pos__))->type !=
           TOKEN_EOF);
}

void Parser_Error(String fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[Parsing Error] ");
  vprintf(fmt, args);
  printf("\n");
  va_end(args);
  exit(1);
}