#ifndef PARSER_H
#define PARSER_H

#include <stdarg.h>

#include "dae/lexer.h"
#include "dae/node.h"
#include "dae/string.h"
#include "dae/vector.h"

typedef Node* (*NativeFunction)(TokenVector*);

typedef struct {
  String name;
  NativeFunction fn;
} NativeFunctionEntry;

typedef struct {
  TokenVector* tokens;
  NodeVector* functions;
  NodeVector* nativeFunctions;
  unsigned int __pos__;
} Parser;

Parser* Parser_New(TokenVector*);

void Parser_Delete(Parser*);

void Parser_DeleteArgs(StringVector*);

Token* Parser_Consume(Parser*, TokenType);

Node* Parser_FindFunction(Parser*, String);

void Parser_RegisterNativeFunction(Parser*, String, NativeFunction);

NativeFunction Parser_FindNativeFunction(Parser*, String);

String Parser_GetTypeFromToken(TokenType);

Node* Parser_ParseFunction(Parser*);

Node* Parser_ParseStatement(Parser*);

void Parser_ParseProgram(Parser*);

void Parser_Error(String, ...);

#endif