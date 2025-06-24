#include "kilate/interpreter.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "kilate/error.h"
#include "kilate/hashmap.h"
#include "kilate/node.h"
#include "kilate/parser.h"
#include "kilate/scope_stack.h"

Interpreter* Interpreter_New(NodeVector* funcNodes,
                             NodeVector* nativeFunctionsNodes) {
  assert(funcNodes);
  assert(nativeFunctionsNodes);
  Interpreter* interpreter = malloc(sizeof(Interpreter));
  interpreter->functions = HashMap_New(sizeof(Node*));
  interpreter->nativeFunctions = HashMap_New(sizeof(NativeFunction));

  // register all funcs
  for (int i = 0; i < funcNodes->size; i++) {
    Node** nodePtr = (Node**)Vector_Get(funcNodes, i);
    if (nodePtr != NULL) {
      Node* node = *nodePtr;
      if (node->type == NODE_FUNCTION) {
        HashMap_Put(interpreter->functions, node->function_n.name, &node);
      }
    }
  }

  for (int i = 0; i < nativeFunctionsNodes->size; i++) {
    NativeFunctionEntry** entryPtr =
        (NativeFunctionEntry**)Vector_Get(nativeFunctionsNodes, i);
    if (entryPtr != NULL) {
      NativeFunctionEntry* entry = *entryPtr;
      HashMap_Put(interpreter->nativeFunctions, entry->name, &entry->fn);
    }
  }

  interpreter->varStack = ScopeStack_New();

  return interpreter;
}

void Interpreter_Delete(Interpreter* self) {
  HashMap_Delete(self->functions);
  HashMap_Delete(self->nativeFunctions);
  ScopeStack_Delete(self->varStack);
  free(self);
}

InterpreterResult Interpreter_Run(Interpreter* self) {
  assert(self);

  Node** mainPtr = (Node**)HashMap_Get(self->functions, "main");
  if (mainPtr == NULL) {
    Error_Fatal("Your program needs a main function!");
  }
  Node* main = *mainPtr;

  if (main->function_n.returnType == NULL ||
      !String_Equals(main->function_n.returnType, "bool")) {
    Error_Fatal("Main function should return bool.");
  }

  return Interpreter_RunFunc(self, main, NULL);
}

InterpreterResult Interpreter_RunFunc(Interpreter* self,
                                      Node* func,
                                      NodeFuncParamVector* params) {
  assert(self);
  assert(func);
  assert(func->type == NODE_FUNCTION);

  ScopeStack_Push(self->varStack);

  if (params != NULL && func->function_n.params != NULL) {
    for (int i = 0; i < params->size; i++) {
      NodeFunctionParam* param = *(NodeFunctionParam**)Vector_Get(params, i);
      NodeFunctionParam* fnParam =
          *(NodeFunctionParam**)Vector_Get(func->function_n.params, i);

      NodeValueType actualType = param->type;
      void* actualValue = param->value;

      if (param->type == NODE_VALUE_TYPE_VAR) {
        Node* realVar = ScopeStack_Get(self->varStack, (String)param->value);
        if (realVar == NULL) {
          Error_Fatal("Variable not defined: %s", (String)param->value);
        }
        actualType = Parser_StringToNodeValueType(realVar->vardec_n.varType);
        actualValue = realVar->vardec_n.varValue;
      }

      if (fnParam->type != NODE_VALUE_TYPE_ANY && fnParam->type != actualType) {
        Error_Fatal(
            "Argument %d to function '%s' expected type '%s', but got '%s'",
            i + 1, func->function_n.name,
            Parser_NodeValueTypeToString(fnParam->type),
            Parser_NodeValueTypeToString(actualType));
      }

      Node* var = VarDecNode_New(fnParam->value,
                                 Parser_NodeValueTypeToString(fnParam->type),
                                 actualType, actualValue);
      ScopeStack_Set(self->varStack, var->vardec_n.varName, var);
    }
  }

  for (int i = 0; i < func->function_n.body->size; i++) {
    Node** stmtPtr = (Node**)Vector_Get(func->function_n.body, i);
    if (stmtPtr != NULL) {
      Node* stmt = *stmtPtr;
      InterpreterResult result = Interpreter_RunNode(self, stmt);
      if (result.type == IRT_RETURN) {
        ScopeStack_Pop(self->varStack);
        return result;
      }
    }
  }

  ScopeStack_Pop(self->varStack);

  // default value
  return (InterpreterResult){.type = IRT_FUNC, .data = NULL};
}

InterpreterResult Interpreter_RunNode(Interpreter* self, Node* node) {
  assert(self);
  assert(node);

  switch (node->type) {
    case NODE_CALL: {
      Node** calledPtr =
          (Node**)HashMap_Get(self->functions, node->call_n.functionName);
      NativeFunction* nativeFnPtr = (NativeFunction*)HashMap_Get(
          self->nativeFunctions, node->call_n.functionName);

      if (calledPtr != NULL) {
        Node* called = *calledPtr;
        InterpreterResult result =
            Interpreter_RunFunc(self, called, node->call_n.functionParams);
        return result;
      } else if (nativeFnPtr != NULL) {
        NativeFnData* nativeFnData = malloc(sizeof(NativeFnData));
        nativeFnData->params = node->call_n.functionParams;
        nativeFnData->varStack = self->varStack;

        NativeFunction nativeFn = *nativeFnPtr;
        Node* nativeFnResult = nativeFn(nativeFnData);
        InterpreterResult result =
            (InterpreterResult){.data = nativeFnResult, .type = IRT_FUNC};
        return result;
      } else {
        Error_Fatal("Function not found: %s", node->call_n.functionName);
      }
    }

    case NODE_RETURN: {
      void* value = NULL;
      if (node->return_n.returnValue != NULL) {
        value = node->return_n.returnValue;  // or evaluate this node if needed
      }
      return (InterpreterResult){.type = IRT_RETURN, .data = value};
    }

    case NODE_VARDEC: {
      /*printf("[DEBUG] Declaring variable '%s' of type '%s' with value '%s'\n",
             node->vardec_n.varName, node->vardec_n.varType,
             (String)node->vardec_n.varValue);*/
      ScopeStack_Set(self->varStack, node->vardec_n.varName, node);
      return (InterpreterResult){.type = IRT_FUNC, .data = NULL};
    }

    default:
      Error_Fatal("Unknown node type %d", node->type);
  }
  return (InterpreterResult){.type = IRT_FUNC, .data = NULL};
}