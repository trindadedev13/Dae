#include "kilate/node.h"

#include <stdlib.h>
#include <string.h>

#include "kilate/string.h"
#include "kilate/vector.h"

Node* FunctionNode_New(String name,
                       String returnType,
                       NodeVector* body,
                       NodeFuncParamVector* params) {
  Node* node = malloc(sizeof(Node));
  node->type = NODE_FUNCTION;
  node->function_n.name = strdup(name);
  if (returnType != NULL) {
    node->function_n.returnType = strdup(returnType);
  } else {
    node->function_n.returnType = NULL;
  }
  node->function_n.body = body;
  node->function_n.params = params;
  return node;
}

Node* CallNode_New(const String functionName,
                   NodeFuncParamVector* functionParams) {
  Node* node = malloc(sizeof(Node));
  node->type = NODE_CALL;
  node->call_n.functionName = strdup(functionName);
  node->call_n.functionParams = functionParams;
  return node;
}

Node* ReturnNode_New(NodeValueType returnType, void* returnValue) {
  Node* node = malloc(sizeof(Node));
  node->type = NODE_RETURN;
  node->return_n.returnType = returnType;
  node->return_n.returnValue = returnValue;
  return node;
}

Node* VarDecNode_New(String name,
                     String type,
                     NodeValueType valueType,
                     void* value) {
  Node* node = malloc(sizeof(Node));
  node->type = NODE_VARDEC;
  node->vardec_n.varName = strdup(name);
  node->vardec_n.varType = strdup(type);
  node->vardec_n.varValueType = valueType;
  node->vardec_n.varValue = value;
  return node;
}