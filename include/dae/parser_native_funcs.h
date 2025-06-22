#ifndef PARSER_NATIVE_FUNCS
#define PARSER_NATIVE_FUNCS

#include "dae/lexer.h"
#include "dae/node.h"
#include "dae/scope_stack.h"
#include "dae/vector.h"

typedef struct {
  ScopeStack* varStack;
  NodeFuncParamVector* params;
} NativeFnData;

String Parser_Native_Internal_InterpretEscapes(String);

Node* Parser_Native_Print(NativeFnData*);

Node* Parser_Native_System(NativeFnData*);

#endif