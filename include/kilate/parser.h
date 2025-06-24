#ifndef PARSER_H
#define PARSER_H

#include <stdarg.h>

#include "kilate/lexer.h"
#include "kilate/node.h"
#include "kilate/parser_native_funcs.h"
#include "kilate/string.h"
#include "kilate/vector.h"

typedef Node* (*NativeFunction)(NativeFnData*);

typedef struct {
  String name;
  StringVector* requiredParams;
  NativeFunction fn;
} NativeFunctionEntry;

typedef struct {
  TokenVector* tokens;
  NodeVector* functions;
  NodeVector* nativeFunctions;
  size_t __pos__;
} Parser;

Parser* Parser_New(TokenVector*);

void Parser_Delete(Parser*);

void Parser_DeleteParams(StringVector*);

Token* Parser_Consume(Parser*, TokenType);

Node* Parser_FindFunction(Parser*, String);

void Parser_Native_RegisterFunction(Parser* parser,
                                    String,
                                    StringVector*,
                                    NativeFunction);

NativeFunctionEntry* Parser_Native_FindFunction(Parser*, String);

void Parser_Native_RegisterAllFunctions(Parser*);

String Parser_TokenTypeToString(TokenType);

String Parser_NodeValueTypeToString(NodeValueType);

NodeValueType Parser_TokenTypeToNodeValueType(Parser*, Token*);

NodeValueType Parser_StringToNodeValueType(String);

Node* Parser_ParseFunction(Parser*);

Node* Parser_ParseStatement(Parser*);

void Parser_ParseProgram(Parser*);

void Parser_Error(Token*, String, ...);

#endif