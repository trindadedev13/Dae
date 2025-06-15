#ifndef PARSER_NATIVE_FUNCS
#define PARSER_NATIVE_FUNCS

#include "dae/lexer.h"
#include "dae/node.h"
#include "dae/vector.h"

String Parser_Native_Internal_InterpretEscapes(String);

Node* Parser_Native_Print(StringVector*);

#endif