#include "dae/parser.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>

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
    // Of course its function node, but its good to check
    if (node->type == NODE_FUNCTION) {
      free(node->function_n.name);
      if (node->function_n.returnType != NULL) {
        free(node->function_n.returnType);
      }
      // free body nodes
      for (size_t j = 0; j < node->function_n.body->size; ++j) {
        Node* bodyNode = *(Node**)Vector_Get(node->function_n.body, j);
        if (bodyNode->type == NODE_CALL) {
          free(bodyNode->call_n.functionName);
          Parser_DeleteParams(bodyNode->call_n.functionParams);
        } else if (bodyNode->type == NODE_VARDEC) {
          free(bodyNode->vardec_n.varName);
          free(bodyNode->vardec_n.varType);
        }
        free(bodyNode);
      }
      Vector_Delete(node->function_n.body);
      // free param nodes
      for (size_t j = 0; j < node->function_n.params->size; ++j) {
        NodeFunctionParam* param =
            *(NodeFunctionParam**)Vector_Get(node->function_n.params, j);
        free(param->value);
        // free(param->typeStr);
        free(param);
      }
      Vector_Delete(node->function_n.params);
    }
    free(node);
  }
  for (size_t i = 0; i < parser->nativeFunctions->size; ++i) {
    NativeFunctionEntry* entry =
        *(NativeFunctionEntry**)Vector_Get(parser->nativeFunctions, i);
    free(entry->name);
    Vector_Delete(entry->requiredParams);
    free(entry);
  }
  Vector_Delete(parser->functions);
  Vector_Delete(parser->nativeFunctions);
  free(parser);
}

void Parser_DeleteParams(NodeFuncParamVector* params) {
  if (params == NULL)
    return;
  for (size_t i = 0; i < params->size; ++i) {
    NodeFunctionParam* param = *(NodeFunctionParam**)Vector_Get(params, i);
    free(param->value);
    // free(param->typeStr);
    free(param);
  }
  Vector_Delete(params);
}

Token* Parser_Consume(Parser* parser, TokenType exType) {
  Token* token = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
  if (token->type != exType) {
    Parser_Error(token, "Expected %s, but got %s", Token_GetTypeName(exType),
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

void Parser_Native_RegisterFunction(Parser* parser,
                                    String name,
                                    StringVector* requiredParams,
                                    NativeFunction fn) {
  NativeFunctionEntry* entry = malloc(sizeof(NativeFunctionEntry));
  entry->name = strdup(name);
  entry->fn = fn;
  entry->requiredParams = requiredParams;
  Vector_PushBack(parser->nativeFunctions, &entry);
}

NativeFunctionEntry* Parser_Native_FindFunction(Parser* parser, String name) {
  for (size_t i = 0; i < parser->nativeFunctions->size; ++i) {
    NativeFunctionEntry* entry =
        *(NativeFunctionEntry**)Vector_Get(parser->nativeFunctions, i);
    if (String_Equals(entry->name, name)) {
      return entry;
    }
  }
  return NULL;
}

void Parser_Native_RegisterAllFunctions(Parser* parser) {
  {
    // Register native print method
    StringVector* requiredParams = Vector_New(sizeof(String*));
    String any = "any";
    Vector_PushBack(requiredParams, &any);
    Parser_Native_RegisterFunction(parser, "print", requiredParams,
                                   Parser_Native_Print);
  }
}

String Parser_TokenTypeToString(TokenType type) {
  switch (type) {
    case TOKEN_STRING:
      return "string";
    case TOKEN_BOOL:
      return "bool";
    case TOKEN_NUMBER:
      return "number";
    default:
      return "unknow";
  }
}

String Parser_NodeValueTypeToString(NodeValueType type) {
  switch (type) {
    case NODE_VALUE_TYPE_NUMBER:
      return "number";
    case NODE_VALUE_TYPE_STRING:
      return "string";
    case NODE_VALUE_TYPE_BOOL:
      return "bool";
    case NODE_VALUE_TYPE_FUNC:
      return "function";
    case NODE_VALUE_TYPE_VAR:
      return "var";
    default:
      return "any";
  }
}

NodeValueType Parser_TokenTypeToNodeValueType(Parser* parser, Token* tk) {
  switch (tk->type) {
    case TOKEN_STRING:
      return NODE_VALUE_TYPE_STRING;
    case TOKEN_BOOL:
      return NODE_VALUE_TYPE_BOOL;
    case TOKEN_NUMBER:
      return NODE_VALUE_TYPE_NUMBER;
    case TOKEN_IDENTIFIER: {
      Token* next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
      if (next->type == TOKEN_LPAREN) {
        return NODE_VALUE_TYPE_FUNC;
      } else {
        return NODE_VALUE_TYPE_VAR;
      }
    }
    case TOKEN_TYPE: {
      if (String_Equals(tk->text, "any")) {
        return NODE_VALUE_TYPE_ANY;
      }
    }
    default:
      return NODE_VALUE_TYPE_ANY;
  }
}

NodeValueType Parser_StringToNodeValueType(String value) {
#ifndef ct
#define ct(str) String_Equals(value, str)
#endif

  if (ct("string")) {
    return NODE_VALUE_TYPE_STRING;
  } else if (ct("bool")) {
    return NODE_VALUE_TYPE_BOOL;
  } else if (ct("number")) {
    return NODE_VALUE_TYPE_NUMBER;
  } else {
    return NODE_VALUE_TYPE_ANY;
  }

#ifdef ct
#undef ct
#endif
}

Node* Parser_ParseStatement(Parser* parser) {
  Token* token = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
  if (String_Equals(token->text, "return")) {
    Parser_Consume(parser, TOKEN_KEYWORD);
    Parser_Consume(parser, TOKEN_ARROW);
    Token* next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
    void* value;
    NodeValueType type;
    if (next->type == TOKEN_BOOL) {
      bool rawBool = false;
      String boolStr = Parser_Consume(parser, TOKEN_BOOL)->text;
      if (String_Equals(boolStr, "true"))
        rawBool = true;
      value = (void*)(intptr_t)rawBool;
      type = NODE_VALUE_TYPE_BOOL;
    } else if (next->type == TOKEN_NUMBER) {
      value = (void*)(intptr_t)String_ToInt(
          Parser_Consume(parser, TOKEN_NUMBER)->text);
      type = NODE_VALUE_TYPE_NUMBER;
    } else if (next->type == TOKEN_STRING) {
      value = Parser_Consume(parser, TOKEN_STRING)->text;
      type = NODE_VALUE_TYPE_STRING;
    } else if (next->type == TOKEN_IDENTIFIER) {
      String name = Parser_Consume(parser, TOKEN_IDENTIFIER)->text;
      Token* next2 = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
      value = name;
      if (next2->type == TOKEN_LPAREN) {
        type = NODE_VALUE_TYPE_FUNC;
      } else {
        type = NODE_VALUE_TYPE_VAR;
      }
      parser->__pos__ += 2;
    } else {
      Parser_Error(next, "Unsupported value in typed return statement.");
      return NULL;
    }
    return ReturnNode_New(type, value);
  } else if (token->type == TOKEN_VAR || token->type == TOKEN_LET) {
    parser->__pos__++;
    String varName = Parser_Consume(parser, TOKEN_IDENTIFIER)->text;
    Parser_Consume(parser, TOKEN_ASSIGN);
    Token* varValueTk = *(Token**)Vector_Get(parser->tokens, parser->__pos__);

    NodeValueType varValueType;
    String varInferredType;
    void* varValue;

    if (varValueTk->type == TOKEN_STRING) {
      varValue = Parser_Consume(parser, TOKEN_STRING)->text;
      varValueType = NODE_VALUE_TYPE_STRING;
      varInferredType = Parser_NodeValueTypeToString(varValueType);
    } else if (varValueTk->type == TOKEN_NUMBER) {
      int temp = String_ToInt(Parser_Consume(parser, TOKEN_NUMBER)->text);
      varValue = (void*)(intptr_t)temp;
      varValueType = NODE_VALUE_TYPE_NUMBER;
      varInferredType = Parser_NodeValueTypeToString(varValueType);
    } else if (varValueTk->type == TOKEN_BOOL) {
      bool rawBool = false;
      String boolStr = Parser_Consume(parser, TOKEN_BOOL)->text;
      if (String_Equals(boolStr, "true"))
        rawBool = true;
      varValue = (void*)(intptr_t)rawBool;
      varValueType = NODE_VALUE_TYPE_BOOL;
      varInferredType = Parser_NodeValueTypeToString(varValueType);
    } else if (varValueTk->type == TOKEN_IDENTIFIER) {
      varValue = Parser_Consume(parser, TOKEN_IDENTIFIER)->text;
      Token* next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
      if (next->type == TOKEN_LPAREN) {
        varValueType = NODE_VALUE_TYPE_FUNC;
      } else {
        varValueType = NODE_VALUE_TYPE_VAR;
      }
      varInferredType = Parser_NodeValueTypeToString(varValueType);
      parser->__pos__ += 2;
    }
    return VarDecNode_New(varName, varInferredType, varValueType, varValue);
  } else if (token->type == TOKEN_TYPE) {
    String varType = Parser_Consume(parser, TOKEN_TYPE)->text;
    String varName = Parser_Consume(parser, TOKEN_IDENTIFIER)->text;
    Parser_Consume(parser, TOKEN_ASSIGN);
    Token* valueTk = *(Token**)Vector_Get(parser->tokens, parser->__pos__);

    NodeValueType varValueType;
    void* varValue;

    if (valueTk->type == TOKEN_STRING) {
      valueTk = Parser_Consume(parser, TOKEN_STRING);
      varValue = valueTk->text;
      varValueType = NODE_VALUE_TYPE_STRING;
    } else if (valueTk->type == TOKEN_NUMBER) {
      valueTk = Parser_Consume(parser, TOKEN_NUMBER);
      int temp = String_ToInt(valueTk->text);
      varValue = (void*)(intptr_t)temp;
      varValueType = NODE_VALUE_TYPE_NUMBER;
    } else if (valueTk->type == TOKEN_BOOL) {
      valueTk = Parser_Consume(parser, TOKEN_BOOL);
      String boolStr = valueTk->text;
      bool bval = String_Equals(boolStr, "true");
      varValue = (void*)(intptr_t)bval;
      varValueType = NODE_VALUE_TYPE_BOOL;
    } else if (valueTk->type == TOKEN_IDENTIFIER) {
      valueTk = Parser_Consume(parser, TOKEN_NUMBER);
      String name = valueTk->text;
      Token* next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);
      varValue = name;
      if (next->type == TOKEN_LPAREN) {
        varValueType = NODE_VALUE_TYPE_FUNC;
      } else {
        varValueType = NODE_VALUE_TYPE_VAR;
      }
      parser->__pos__ += 2;
    } else {
      Parser_Error(valueTk, "Unsupported value in typed variable declaration.");
      return NULL;
    }

    String expected = varType;
    String actual = Parser_TokenTypeToString(valueTk->type);
    if (!String_Equals(expected, "any") && !String_Equals(expected, actual)) {
      Parser_Error(
          valueTk,
          "Type mismatch in declaration of '%s': expected '%s', got '%s'",
          varName, expected, actual);
    }

    return VarDecNode_New(varName, varType, varValueType, varValue);
  } else if (token->type == TOKEN_IDENTIFIER) {
    String name = Parser_Consume(parser, TOKEN_IDENTIFIER)->text;
    Token* next = *(Token**)Vector_Get(parser->tokens, parser->__pos__);

    // call with () no params
    if (next->type == TOKEN_LPAREN) {
      Parser_Consume(parser, TOKEN_LPAREN);
      Parser_Consume(parser, TOKEN_RPAREN);

      Node* fn = Parser_FindFunction(parser, name);
      if (fn != NULL) {
        size_t expected = fn->function_n.params->size;
        if (expected > 0) {
          Parser_Error(
              next,
              "Function '%s' expects %zu parameters but none were provided.",
              name, expected);
        }
      } else {
        NativeFunctionEntry* nativeFn =
            Parser_Native_FindFunction(parser, name);
        if (nativeFn != NULL) {
          if (nativeFn->requiredParams->size > 0) {
            Parser_Error(next,
                         "Native function '%s' expects %zu parameters but none "
                         "were provided.",
                         name, nativeFn->requiredParams->size);
          }
        } else {
          Parser_Error(
              token,
              "Function '%s' is not declared and is not a native function.",
              name);
        }
      }

      return CallNode_New(name, NULL);
    }

    // call with ->
    if (next->type == TOKEN_ARROW) {
      Parser_Consume(parser, TOKEN_ARROW);
      NodeFuncParamVector* params = Vector_New(sizeof(NodeFuncParamVector*));

      while (true) {
        Token* param = *(Token**)Vector_Get(parser->tokens, parser->__pos__);

        if (param->type != TOKEN_STRING && param->type != TOKEN_BOOL &&
            param->type != TOKEN_NUMBER && param->type != TOKEN_IDENTIFIER) {
          break;
        }

        NodeFunctionParam* fnParam = malloc(sizeof(NodeFunctionParam));
        fnParam->value = strdup(param->text);
        fnParam->type = Parser_TokenTypeToNodeValueType(parser, param);
        // fnParam->typeStr = strdup(Token_GetTypeName(param->type));
        Vector_PushBack(params, &fnParam);

        Parser_Consume(parser, param->type);

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
        if (params->size != expected) {
          Parser_Error(next,
                       "Function '%s' expects %zu parameters but got %zu.",
                       name, expected, params->size);
        }

        for (size_t i = 0; i < expected; ++i) {
          NodeFunctionParam* param =
              *(NodeFunctionParam**)Vector_Get(fn->function_n.params, i);

          NodeFunctionParam* callParam =
              *(NodeFunctionParam**)Vector_Get(params, i);
          NodeValueType actualType = callParam->type;
          if (actualType == NODE_VALUE_TYPE_VAR || NODE_VALUE_TYPE_FUNC)
            continue;
          if (param->type != NODE_VALUE_TYPE_ANY && param->type != actualType) {
            Parser_Error(token,
                         "Argument %zu to '%s' must be of type %s, got %s",
                         i + 1, name, Parser_NodeValueTypeToString(param->type),
                         Parser_NodeValueTypeToString(actualType));
          }
        }
        return CallNode_New(name, params);
      }

      // check if its native
      NativeFunctionEntry* nativeFn = Parser_Native_FindFunction(parser, name);
      if (nativeFn != NULL) {
        return CallNode_New(name, params);
      }

      Parser_Error(
          token, "Function '%s' is not declared and is not a native function.",
          name);
    }

    Parser_Error(token, "Unexpected token after identifier: %s", next->text);
  }
  Parser_Error(token, "Unknown statement: %s", token->text);
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
    param->value = strdup(name);
    param->type = Parser_StringToNodeValueType(type);
    // param->typeStr = strdup(type);
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

  if (body->size == 0) {
    Parser_Error((*(Token**)Vector_Get(parser->tokens, parser->__pos__)),
                 "Function '%s' is empty, remove or implement it.", name);
  }

  if (returnType != NULL) {
    Node* lastNode = *(Node**)Vector_Get(body, body->size - 1);
    if (lastNode != NULL) {
      if (lastNode->type != NODE_RETURN) {
        Parser_Error((*(Token**)Vector_Get(parser->tokens, parser->__pos__)),
                     "Function '%s' must end with return statement.", name);
      }
      NodeValueType retType;
      if (String_Equals(returnType, "number")) {
        retType = NODE_VALUE_TYPE_NUMBER;
      } else if (String_Equals(returnType, "string")) {
        retType = NODE_VALUE_TYPE_STRING;
      } else if (String_Equals(returnType, "bool")) {
        retType = NODE_VALUE_TYPE_BOOL;
      } else if (String_Equals(returnType, "any")) {
        retType = NODE_VALUE_TYPE_ANY;
      } else {
        retType = lastNode->return_n.returnType;
      }
      if (retType != lastNode->return_n.returnType) {
        Parser_Error(
            (*(Token**)Vector_Get(parser->tokens, parser->__pos__)),
            "The expected return type of function '%s' is '%s', but what was "
            "received was: '%s'",
            name, returnType, Parser_NodeValueTypeToString(retType));
      }
    }
  }

  return FunctionNode_New(name, returnType, body, params);
}

void Parser_ParseProgram(Parser* parser) {
  Parser_Native_RegisterAllFunctions(parser);
  do {
    Node* node = Parser_ParseFunction(parser);
    Vector_PushBack(parser->functions, &node);
  } while ((*(Token**)Vector_Get(parser->tokens, parser->__pos__))->type !=
           TOKEN_EOF);
}

void Parser_Error(Token* tk, String fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[Error At %lu:%lu] ", tk->line, tk->column);
  vprintf(fmt, args);
  printf("\n");
  va_end(args);
  exit(1);
}