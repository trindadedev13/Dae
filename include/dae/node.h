#ifndef NODE_H
#define NODE_H

#include "dae/string.h"
#include "dae/vector.h"

typedef enum { NODE_FUNCTION, NODE_PRINT, NODE_CALL, NODE_RETURN } NodeType;

typedef enum {
  NODE_RETURN_NUMBER,
  NODE_RETURN_STRING,
  NODE_RETURN_BOOL
} NodeReturnType;

typedef struct Node Node;
typedef Vector NodeVector;

typedef struct {
  String name;
  String type;
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
    Vector* functionParams;
  } call_n;

  struct {
    NodeReturnType returnType;
    void* returnValue;
  } return_n;
};

Node* FunctionNode_New(String, String, Vector*, Vector*);

Node* CallNode_New(String, Vector*);

Node* ReturnNode_New(NodeReturnType, void*);

#endif