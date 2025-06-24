#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "kilate/hashmap.h"
#include "kilate/node.h"
#include "kilate/scope_stack.h"
#include "kilate/string.h"
#include "kilate/vector.h"

typedef struct {
  HashMap* functions;
  HashMap* nativeFunctions;
  ScopeStack* varStack;
} Interpreter;

typedef enum { IRT_FUNC, IRT_RETURN } InterpreterResultType;

typedef struct {
  void* data;
  InterpreterResultType type;
} InterpreterResult;

Interpreter* Interpreter_New(NodeVector*, NodeVector*);

void Interpreter_Delete(Interpreter*);

InterpreterResult Interpreter_Run(Interpreter*);

InterpreterResult Interpreter_RunFunc(Interpreter*, Node*, StringVector*);

InterpreterResult Interpreter_RunNode(Interpreter*, Node*);

#endif