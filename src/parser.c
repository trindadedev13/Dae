#include "kilate/parser.h"

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kilate/lexer.h"
#include "kilate/native.h"
#include "kilate/node.h"
#include "kilate/string.h"
#include "kilate/vector.h"

klt_parser* klt_parser_make(klt_token_vector* tokens) {
  klt_parser* parser = malloc(sizeof(klt_parser));
  parser->tokens = tokens;
  parser->functions = klt_vector_make(sizeof(klt_node*));
  parser->__pos__ = 0;
  return parser;
}

void klt_parser_delete(klt_parser* parser) {
  for (size_t i = 0; i < parser->functions->size; ++i) {
    klt_node* node = *(klt_node**)klt_vector_get(parser->functions, i);
    // Of course its function node, but its good to check
    if (node->type == NODE_FUNCTION) {
      free(node->function_n.name);
      if (node->function_n.returnType != NULL) {
        free(node->function_n.returnType);
      }
      // free body nodes
      for (size_t j = 0; j < node->function_n.body->size; ++j) {
        klt_node** bodyklt_nodePtr =
            (klt_node**)klt_vector_get(node->function_n.body, j);
        if (bodyklt_nodePtr != NULL) {
          klt_node* bodyklt_node = *bodyklt_nodePtr;
          if (bodyklt_node->type == NODE_CALL) {
            free(bodyklt_node->call_n.functionName);
            klt_parser_delete_params(bodyklt_node->call_n.functionParams);
          } else if (bodyklt_node->type == NODE_VARDEC) {
            free(bodyklt_node->vardec_n.varName);
            free(bodyklt_node->vardec_n.varType);
          }
          free(bodyklt_node);
        }
      }
      klt_vector_delete(node->function_n.body);
      // free param nodes
      for (size_t j = 0; j < node->function_n.params->size; ++j) {
        klt_node_fnparam* param =
            *(klt_node_fnparam**)klt_vector_get(node->function_n.params, j);
        free(param->value);
        // free(param->typeStr);
        free(param);
      }
      klt_vector_delete(node->function_n.params);
    }
    free(node);
  }
  klt_vector_delete(parser->functions);
  free(parser);
}

void klt_parser_delete_params(klt_node_fnparam_vector* params) {
  if (params == NULL)
    return;
  for (size_t i = 0; i < params->size; ++i) {
    klt_node_fnparam* param = *(klt_node_fnparam**)klt_vector_get(params, i);
    free(param->value);
    // free(param->typeStr);
    free(param);
  }
  klt_vector_delete(params);
}

klt_token* klt_parser_consume(klt_parser* parser, klt_token_type exType) {
  klt_token* token =
      *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
  if (token->type != exType) {
    klt_parser_error(token, "Expected %s, but got %s",
                     klt_tokentype_tostr(exType),
                     klt_tokentype_tostr(token->type));
    return NULL;
  }
  parser->__pos__++;
  return token;
}

klt_node* klt_parser_find_function(klt_parser* parser, klt_str name) {
  for (size_t i = 0; i < parser->functions->size; i++) {
    klt_node* fn = *(klt_node**)klt_vector_get(parser->functions, i);
    if (klt_str_equals(fn->function_n.name, name)) {
      return fn;
    }
  }
  return NULL;
}

klt_str klt_parser_tokentype_to_str(klt_token_type type) {
  switch (type) {
    case TOKEN_STRING:
      return "string";
    case TOKEN_BOOL:
      return "bool";
    case TOKEN_INT:
      return "int";
    case TOKEN_FLOAT:
      return "float";
    case TOKEN_LONG:
      return "long";
    default:
      return "unknow";
  }
}

klt_str klt_parser_nodevaluetype_to_str(klt_node_valuetype type) {
  switch (type) {
    case NODE_VALUE_TYPE_INT:
      return "int";
    case NODE_VALUE_TYPE_FLOAT:
      return "float";
    case NODE_VALUE_TYPE_LONG:
      return "long";
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

klt_node_valuetype klt_parser_tokentype_to_nodevaluetype(klt_parser* parser,
                                                         klt_token* tk) {
  switch (tk->type) {
    case TOKEN_STRING:
      return NODE_VALUE_TYPE_STRING;
    case TOKEN_BOOL:
      return NODE_VALUE_TYPE_BOOL;
    case TOKEN_INT:
      return NODE_VALUE_TYPE_INT;
    case TOKEN_FLOAT:
      return NODE_VALUE_TYPE_FLOAT;
    case TOKEN_LONG:
      return NODE_VALUE_TYPE_LONG;
    case TOKEN_IDENTIFIER: {
      klt_token* next =
          *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
      if (next->type == TOKEN_LPAREN) {
        return NODE_VALUE_TYPE_FUNC;
      } else {
        return NODE_VALUE_TYPE_VAR;
      }
    }
    case TOKEN_TYPE: {
      if (klt_str_equals(tk->text, "any")) {
        return NODE_VALUE_TYPE_ANY;
      }
    }
    default:
      return NODE_VALUE_TYPE_ANY;
  }
}

klt_node_valuetype klt_parser_str_to_nodevaluetype(klt_str value) {
#ifndef ct
#define ct(str) klt_str_equals(value, str)
#endif

  if (ct("string")) {
    return NODE_VALUE_TYPE_STRING;
  } else if (ct("bool")) {
    return NODE_VALUE_TYPE_BOOL;
  } else if (ct("int")) {
    return NODE_VALUE_TYPE_INT;
  } else if (ct("float")) {
    return NODE_VALUE_TYPE_FLOAT;
  } else if (ct("long")) {
    return NODE_VALUE_TYPE_LONG;
  } else {
    return NODE_VALUE_TYPE_ANY;
  }

#ifdef ct
#undef ct
#endif
}

klt_node* klt_parser_parse_statement(klt_parser* parser) {
  klt_token* token =
      *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
  if (klt_str_equals(token->text, "return")) {
    klt_parser_consume(parser, TOKEN_KEYWORD);
    klt_token* arrow =
        *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
    if (arrow->type == TOKEN_RARROW) {
      klt_parser_consume(parser, TOKEN_RARROW);
    } else if (arrow->type == TOKEN_LARROW) {
      klt_parser_consume(parser, TOKEN_LARROW);
    }
    klt_token* next =
        *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
    void* value;
    klt_node_valuetype type;
    if (next->type == TOKEN_BOOL) {
      bool rawBool = false;
      klt_str boolStr = klt_parser_consume(parser, TOKEN_BOOL)->text;
      if (klt_str_equals(boolStr, "true"))
        rawBool = true;
      value = (void*)(intptr_t)rawBool;
      type = NODE_VALUE_TYPE_BOOL;
    } else if (next->type == TOKEN_INT) {
      value = (void*)(intptr_t)klt_str_to_int(
          klt_parser_consume(parser, TOKEN_INT)->text);
      type = NODE_VALUE_TYPE_INT;
    } else if (next->type == TOKEN_FLOAT) {
      float fval =
          klt_str_to_float(klt_parser_consume(parser, TOKEN_FLOAT)->text);
      value = malloc(sizeof(float));
      memcpy(value, &fval, sizeof(float));
      type = NODE_VALUE_TYPE_FLOAT;
    } else if (next->type == TOKEN_LONG) {
      long lval = klt_str_to_long(klt_parser_consume(parser, TOKEN_LONG)->text);
      value = (void*)(intptr_t)lval;
      type = NODE_VALUE_TYPE_LONG;
    } else if (next->type == TOKEN_STRING) {
      value = klt_parser_consume(parser, TOKEN_STRING)->text;
      type = NODE_VALUE_TYPE_STRING;
    } else if (next->type == TOKEN_IDENTIFIER) {
      klt_str name = klt_parser_consume(parser, TOKEN_IDENTIFIER)->text;
      klt_token* next2 =
          *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
      value = name;
      if (next2->type == TOKEN_LPAREN) {
        type = NODE_VALUE_TYPE_FUNC;
      } else {
        type = NODE_VALUE_TYPE_VAR;
      }
      parser->__pos__ += 2;
    } else {
      klt_parser_error(next, "Unsupported value in typed return statement.");
      return NULL;
    }
    return klt_return_node_make(type, value);
  } else if (token->type == TOKEN_VAR || token->type == TOKEN_LET) {
    parser->__pos__++;
    klt_str varName = klt_parser_consume(parser, TOKEN_IDENTIFIER)->text;
    klt_parser_consume(parser, TOKEN_ASSIGN);
    klt_token* varValueTk =
        *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);

    klt_node_valuetype varValueType;
    klt_str varInferredType;
    void* varValue;

    if (varValueTk->type == TOKEN_STRING) {
      varValue = klt_parser_consume(parser, TOKEN_STRING)->text;
      varValueType = NODE_VALUE_TYPE_STRING;
    } else if (varValueTk->type == TOKEN_INT) {
      int temp = klt_str_to_int(klt_parser_consume(parser, TOKEN_INT)->text);
      varValue = (void*)(intptr_t)temp;
      varValueType = NODE_VALUE_TYPE_INT;
    } else if (varValueTk->type == TOKEN_FLOAT) {
      float fval =
          klt_str_to_float(klt_parser_consume(parser, TOKEN_FLOAT)->text);
      varValue = malloc(sizeof(float));
      memcpy(varValue, &fval, sizeof(float));
      varValueType = NODE_VALUE_TYPE_FLOAT;
    } else if (varValueTk->type == TOKEN_LONG) {
      long lval = klt_str_to_long(klt_parser_consume(parser, TOKEN_LONG)->text);
      varValue = (void*)(intptr_t)lval;
      varValueType = NODE_VALUE_TYPE_LONG;
    } else if (varValueTk->type == TOKEN_BOOL) {
      bool rawBool = false;
      klt_str boolStr = klt_parser_consume(parser, TOKEN_BOOL)->text;
      if (klt_str_equals(boolStr, "true"))
        rawBool = true;
      varValue = (void*)(intptr_t)rawBool;
      varValueType = NODE_VALUE_TYPE_BOOL;
    } else if (varValueTk->type == TOKEN_IDENTIFIER) {
      varValue = klt_parser_consume(parser, TOKEN_IDENTIFIER)->text;
      klt_token* next =
          *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
      if (next->type == TOKEN_LPAREN) {
        varValueType = NODE_VALUE_TYPE_FUNC;
      } else {
        varValueType = NODE_VALUE_TYPE_VAR;
      }
      parser->__pos__ += 2;
    }
    varInferredType = klt_parser_nodevaluetype_to_str(varValueType);
    return klt_var_dec_node_make(varName, varInferredType, varValueType,
                                 varValue);
  } else if (token->type == TOKEN_TYPE) {
    klt_str varType = klt_parser_consume(parser, TOKEN_TYPE)->text;
    klt_str varName = klt_parser_consume(parser, TOKEN_IDENTIFIER)->text;
    klt_parser_consume(parser, TOKEN_ASSIGN);
    klt_token* valueTk =
        *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);

    klt_node_valuetype varValueType;
    void* varValue;

    if (valueTk->type == TOKEN_STRING) {
      valueTk = klt_parser_consume(parser, TOKEN_STRING);
      varValue = valueTk->text;
      varValueType = NODE_VALUE_TYPE_STRING;
    } else if (valueTk->type == TOKEN_INT) {
      valueTk = klt_parser_consume(parser, TOKEN_INT);
      int temp = klt_str_to_int(valueTk->text);
      varValue = (void*)(intptr_t)temp;
      varValueType = NODE_VALUE_TYPE_INT;
    } else if (valueTk->type == TOKEN_FLOAT) {
      valueTk = klt_parser_consume(parser, TOKEN_FLOAT);
      float fval = klt_str_to_float(valueTk->text);
      varValue = malloc(sizeof(float));
      memcpy(varValue, &fval, sizeof(float));
      varValueType = NODE_VALUE_TYPE_FLOAT;
    } else if (valueTk->type == TOKEN_LONG) {
      valueTk = klt_parser_consume(parser, TOKEN_LONG);
      long lval = klt_str_to_long(valueTk->text);
      varValue = (void*)(intptr_t)lval;
      varValueType = NODE_VALUE_TYPE_LONG;
    } else if (valueTk->type == TOKEN_BOOL) {
      valueTk = klt_parser_consume(parser, TOKEN_BOOL);
      klt_str boolStr = valueTk->text;
      bool bval = klt_str_equals(boolStr, "true");
      varValue = (void*)(intptr_t)bval;
      varValueType = NODE_VALUE_TYPE_BOOL;
    } else if (valueTk->type == TOKEN_IDENTIFIER) {
      valueTk = klt_parser_consume(parser, TOKEN_IDENTIFIER);
      klt_str name = valueTk->text;
      klt_token* next =
          *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
      varValue = name;
      if (next->type == TOKEN_LPAREN) {
        varValueType = NODE_VALUE_TYPE_FUNC;
      } else {
        varValueType = NODE_VALUE_TYPE_VAR;
      }
      parser->__pos__ += 2;
    } else {
      klt_parser_error(valueTk,
                       "Unsupported value in typed variable declaration.");
      return NULL;
    }

    klt_str expected = varType;
    klt_str actual = klt_parser_tokentype_to_str(valueTk->type);
    if (!klt_str_equals(expected, "any") && !klt_str_equals(expected, actual)) {
      klt_parser_error(
          valueTk,
          "Type mismatch in declaration of '%s': expected '%s', got '%s'",
          varName, expected, actual);
    }

    return klt_var_dec_node_make(varName, varType, varValueType, varValue);
  } else if (token->type == TOKEN_IDENTIFIER) {
    klt_str name = klt_parser_consume(parser, TOKEN_IDENTIFIER)->text;
    klt_token* next =
        *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);

    // call with () no params
    if (next->type == TOKEN_LPAREN) {
      klt_parser_consume(parser, TOKEN_LPAREN);
      klt_parser_consume(parser, TOKEN_RPAREN);

      klt_node* fn = klt_parser_find_function(parser, name);
      if (fn != NULL) {
        size_t expected = fn->function_n.params->size;
        if (expected > 0) {
          klt_parser_error(
              next,
              "Function '%s' expects %zu parameters but none were provided.",
              name, expected);
        }
      } else {
        klt_native_fnentry* nativeFn = klt_native_find_function(name);
        if (nativeFn != NULL) {
          if (nativeFn->requiredParams != NULL &&
              nativeFn->requiredParams->size > 0) {
            klt_parser_error(
                next,
                "Native function '%s' expects %zu parameters but none "
                "were provided.",
                name, nativeFn->requiredParams->size);
          }
        } else {
          klt_parser_error(
              token,
              "Function '%s' is not declared and is not a native function.",
              name);
        }
      }

      return klt_call_node_make(name, NULL);
    }

    // call with ->
    if (next->type == TOKEN_RARROW || next->type == TOKEN_LARROW) {
      klt_parser_consume(parser, next->type);
      klt_node_fnparam_vector* params =
          klt_vector_make(sizeof(klt_node_fnparam_vector*));

      while (true) {
        klt_token* param =
            *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);

        if (param->type != TOKEN_STRING && param->type != TOKEN_BOOL &&
            param->type != TOKEN_INT && param->type != TOKEN_FLOAT &&
            param->type != TOKEN_LONG && param->type != TOKEN_IDENTIFIER) {
          break;
        }

        klt_node_fnparam* fnParam = malloc(sizeof(klt_node_fnparam));
        fnParam->value = strdup(param->text);
        fnParam->type = klt_parser_tokentype_to_nodevaluetype(parser, param);
        // fnParam->typeStr = strdup(klt_tokentype_tostr(param->type));
        klt_vector_push_back(params, &fnParam);

        klt_parser_consume(parser, param->type);

        klt_token* comma =
            *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
        if (comma->type == TOKEN_COMMA) {
          klt_parser_consume(parser, TOKEN_COMMA);
        } else {
          break;
        }
      }

      klt_node* fn = klt_parser_find_function(parser, name);
      if (fn != NULL) {
        size_t expected = fn->function_n.params->size;
        if (params->size != expected) {
          klt_parser_error(next,
                           "Function '%s' expects %zu parameters but got %zu.",
                           name, expected, params->size);
        }

        for (size_t i = 0; i < expected; ++i) {
          klt_node_fnparam* param =
              *(klt_node_fnparam**)klt_vector_get(fn->function_n.params, i);

          klt_node_fnparam* callParam =
              *(klt_node_fnparam**)klt_vector_get(params, i);
          klt_node_valuetype actualType = callParam->type;
          if (actualType == NODE_VALUE_TYPE_VAR || NODE_VALUE_TYPE_FUNC)
            continue;
          if (param->type != NODE_VALUE_TYPE_ANY && param->type != actualType) {
            klt_parser_error(
                token, "Argument %zu to '%s' must be of type %s, got %s", i + 1,
                name, klt_parser_nodevaluetype_to_str(param->type),
                klt_parser_nodevaluetype_to_str(actualType));
          }
        }
        return klt_call_node_make(name, params);
      }

      // check if its native
      klt_native_fnentry* nativeFn = klt_native_find_function(name);
      if (nativeFn != NULL) {
        return klt_call_node_make(name, params);
      }

      klt_parser_error(
          token, "Function '%s' is not declared and is not a native function.",
          name);
    }

    klt_parser_error(token, "Unexpected token after identifier: %s",
                     next->text);
  }
  klt_parser_error(token, "Unknown statement: %s", token->text);
  return NULL;
}

klt_node* klt_parser_parse_function(klt_parser* parser) {
  klt_parser_consume(parser, TOKEN_KEYWORD);
  klt_str name = klt_parser_consume(parser, TOKEN_IDENTIFIER)->text;

  klt_parser_consume(parser, TOKEN_LPAREN);
  klt_node_fnparam_vector* params = klt_vector_make(sizeof(klt_node_fnparam*));

  while (true) {
    klt_token* next =
        *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
    if (next->type == TOKEN_RPAREN) {
      klt_parser_consume(parser, TOKEN_RPAREN);
      break;
    }

    klt_str type = klt_parser_consume(parser, TOKEN_TYPE)->text;
    klt_parser_consume(parser, TOKEN_COLON);
    klt_str name = klt_parser_consume(parser, TOKEN_IDENTIFIER)->text;

    klt_node_fnparam* param = malloc(sizeof(klt_node_fnparam));
    param->value = strdup(name);
    param->type = klt_parser_str_to_nodevaluetype(type);
    // param->typeStr = strdup(type);
    klt_vector_push_back(params, &param);

    next = *(klt_token**)klt_vector_get(parser->tokens, parser->__pos__);
    if (next->type == TOKEN_COMMA) {
      klt_parser_consume(parser, TOKEN_COMMA);
    }
  }

  klt_str returnType = NULL;
  if ((*(klt_token**)klt_vector_get(parser->tokens, parser->__pos__))->type ==
      TOKEN_COLON) {
    klt_parser_consume(parser, TOKEN_COLON);
    returnType = klt_parser_consume(parser, TOKEN_TYPE)->text;
  }

  klt_parser_consume(parser, TOKEN_LBRACE);
  klt_node_vector* body = klt_vector_make(sizeof(klt_node*));
  while (
      (*(klt_token**)klt_vector_get(parser->tokens, parser->__pos__))->type !=
      TOKEN_RBRACE) {
    klt_node* node = klt_parser_parse_statement(parser);
    klt_vector_push_back(body, &node);
  }

  klt_parser_consume(parser, TOKEN_RBRACE);

  if (body->size == 0) {
    klt_parser_error(
        (*(klt_token**)klt_vector_get(parser->tokens, parser->__pos__)),
        "Function '%s' is empty, remove or implement it.", name);
  }

  if (returnType != NULL) {
    klt_node* lastklt_node = *(klt_node**)klt_vector_get(body, body->size - 1);
    if (lastklt_node != NULL) {
      if (lastklt_node->type != NODE_RETURN) {
        klt_parser_error(
            (*(klt_token**)klt_vector_get(parser->tokens, parser->__pos__)),
            "Function '%s' must end with return statement.", name);
      }
      klt_node_valuetype retType;
      if (klt_str_equals(returnType, "int")) {
        retType = NODE_VALUE_TYPE_INT;
      } else if (klt_str_equals(returnType, "float")) {
        retType = NODE_VALUE_TYPE_FLOAT;
      } else if (klt_str_equals(returnType, "long")) {
        retType = NODE_VALUE_TYPE_LONG;
      } else if (klt_str_equals(returnType, "string")) {
        retType = NODE_VALUE_TYPE_STRING;
      } else if (klt_str_equals(returnType, "bool")) {
        retType = NODE_VALUE_TYPE_BOOL;
      } else if (klt_str_equals(returnType, "any")) {
        retType = NODE_VALUE_TYPE_ANY;
      } else {
        retType = lastklt_node->return_n.returnType;
      }
      if (retType != lastklt_node->return_n.returnType) {
        klt_parser_error(
            (*(klt_token**)klt_vector_get(parser->tokens, parser->__pos__)),
            "The expected return type of function '%s' is '%s', but what was "
            "received was: '%s'",
            name, returnType, klt_parser_nodevaluetype_to_str(retType));
      }
    }
  }

  return klt_function_node_make(name, returnType, body, params);
}

void klt_parser_parse_program(klt_parser* parser) {
  do {
    klt_node* node = klt_parser_parse_function(parser);
    klt_vector_push_back(parser->functions, &node);
  } while (
      (*(klt_token**)klt_vector_get(parser->tokens, parser->__pos__))->type !=
      TOKEN_EOF);
}

void klt_parser_error(klt_token* tk, klt_str fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[Error At %zu:%zu] ", tk->line, tk->column);
  vprintf(fmt, args);
  printf("\n");
  va_end(args);
  exit(1);
}