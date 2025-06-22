#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "dae/hashmap.h"
#include "dae/node.h"
#include "dae/scope_stack.h"
#include "dae/string.h"
#include "dae/vector.h"

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