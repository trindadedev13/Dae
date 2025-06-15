#include "dae/node.h"

#include <stdlib.h>

#include "dae/string.h"
#include "dae/vector.h"

Node* FunctionNode_New(String name,
                       String returnType,
                       NodeVector* body,
                       Vector* params) {
  Node* node = malloc(sizeof(Node));
  node->type = NODE_FUNCTION;
  node->function_n.name = name;
  node->function_n.returnType = returnType;
  node->function_n.body = body;
  node->function_n.params = params;
  return node;
}

Node* CallNode_New(const String functionName, Vector* functionParams) {
  Node* node = malloc(sizeof(Node));
  node->type = NODE_CALL;
  node->call_n.functionName = functionName;
  node->call_n.functionParams = functionParams;
  return node;
}

Node* ReturnNode_New(NodeReturnType returnType, void* returnValue) {
  Node* node = malloc(sizeof(Node));
  node->type = NODE_RETURN;
  node->return_n.returnType = returnType;
  node->return_n.returnValue = returnValue;
  return node;
}