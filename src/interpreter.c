#include "dae/interpreter.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "dae/error.h"
#include "dae/hashmap.h"
#include "dae/node.h"
#include "dae/parser.h"

Interpreter* Interpreter_New(NodeVector* funcNodes,
                             NodeVector* nativeFunctionsNodes) {
  assert(funcNodes);
  assert(nativeFunctionsNodes);
  Interpreter* interpreter = malloc(sizeof(Interpreter));
  interpreter->functions = HashMap_New(sizeof(Node*));
  interpreter->nativeFunctions = HashMap_New(sizeof(NativeFunction));

  // register all funcs
  for (int i = 0; i < funcNodes->size; i++) {
    Node* node = *(Node**)Vector_Get(funcNodes, i);
    if (node->type == NODE_FUNCTION) {
      HashMap_Put(interpreter->functions, node->function_n.name, &node);
    }
  }

  for (int i = 0; i < nativeFunctionsNodes->size; i++) {
    NativeFunctionEntry* entry =
        *(NativeFunctionEntry**)Vector_Get(nativeFunctionsNodes, i);
    HashMap_Put(interpreter->nativeFunctions, entry->name, &entry->fn);
  }

  return interpreter;
}

void Interpreter_Delete(Interpreter* self) {
  HashMap_Delete(self->functions);
  HashMap_Delete(self->nativeFunctions);
  free(self);
}

InterpreterResult Interpreter_Run(Interpreter* self) {
  assert(self);

  Node** mainPtr = (Node**)HashMap_Get(self->functions, "main");
  if (mainPtr == NULL) {
    Error_Fatal("Your program needs a main function!\n");
  }
  Node* main = *mainPtr;

  return Interpreter_RunFunc(self, main);
}

InterpreterResult Interpreter_RunFunc(Interpreter* self, Node* func) {
  assert(self);
  assert(func);
  assert(func->type == NODE_FUNCTION);

  for (int i = 0; i < func->function_n.body->size; i++) {
    Node* stmt = *(Node**)Vector_Get(func->function_n.body, i);
    InterpreterResult result = Interpreter_RunNode(self, stmt);
    if (result.type == IRT_RETURN) {
      return result;
    }
  }

  // default value
  InterpreterResult ret = {.type = IRT_RETURN, .data = NULL};
  return ret;
}

InterpreterResult Interpreter_RunNode(Interpreter* self, Node* node) {
  assert(self);
  assert(node);

  switch (node->type) {
    case NODE_CALL: {
      Node** calledPtr =
          (Node**)HashMap_Get(self->functions, node->call_n.functionName);
      if (calledPtr != NULL) {
        Node* called = *calledPtr;
        return Interpreter_RunFunc(self, called);
      }
      NativeFunction* nativeFnPtr = (NativeFunction*)HashMap_Get(
          self->nativeFunctions, node->call_n.functionName);
      if (nativeFnPtr != NULL) {
        NativeFunction nativeFn = *nativeFnPtr;
        return (InterpreterResult){
            .data = nativeFn(node->call_n.functionParams), .type = IRT_FUNC};
      }

      Error_Fatal("Function not found: %s\n", node->call_n.functionName);
    }

    case NODE_RETURN: {
      return (InterpreterResult){.type = IRT_RETURN,
                                 .data = node->return_n.returnValue};
    }

    default:
      Error_Fatal("Unknown node type %d\n", node->type);
  }
  return (InterpreterResult){.type = IRT_RETURN, .data = NULL};
}