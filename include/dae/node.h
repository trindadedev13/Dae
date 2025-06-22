#ifndef NODE_H
#define NODE_H

#include "dae/lexer.h"
#include "dae/string.h"
#include "dae/vector.h"

typedef enum {
  NODE_FUNCTION,
  NODE_PRINT,
  NODE_CALL,
  NODE_RETURN,
  NODE_VARDEC
} NodeType;

typedef enum {
  NODE_VALUE_TYPE_NUMBER,
  NODE_VALUE_TYPE_STRING,
  NODE_VALUE_TYPE_BOOL,
  NODE_VALUE_TYPE_VAR,
  NODE_VALUE_TYPE_FUNC,
  NODE_VALUE_TYPE_ANY
} NodeValueType;

typedef struct Node Node;
typedef Vector NodeVector;

typedef struct {
  String value;
  // String typeStr;
  NodeValueType type;
} NodeFunctionParam;

typedef Vector NodeFuncParamVector;

struct Node {
  NodeType type;

  struct {
    String name;
    String returnType;
    NodeVector* body;
    NodeFuncParamVector* params;
  } function_n;

  struct {
    String functionName;
    NodeFuncParamVector* functionParams;
  } call_n;

  struct {
    NodeValueType returnType;
    void* returnValue;
  } return_n;

  struct {
    String varName;
    String varType;
    NodeValueType varValueType;
    void* varValue;
  } vardec_n;
};

Node* FunctionNode_New(String, String, NodeVector*, NodeFuncParamVector*);

Node* CallNode_New(String, NodeFuncParamVector*);

Node* ReturnNode_New(NodeValueType, void*);

Node* VarDecNode_New(String, String, NodeValueType, void*);

#endif