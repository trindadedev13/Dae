#ifndef LEXER_H
#define LEXER_H

#include "dae/inline.h"
#include "dae/string.h"
#include "dae/vector.h"

typedef enum {
  TOKEN_KEYWORD,     // work, return
  TOKEN_IDENTIFIER,  // hello, main
  TOKEN_STRING,      // "Hello world!"
  TOKEN_LPAREN,      // (
  TOKEN_RPAREN,      // )
  TOKEN_LBRACE,      // {
  TOKEN_RBRACE,      // }
  TOKEN_ARROW,       // ->
  TOKEN_COLON,       // :
  TOKEN_TYPE,        // bool
  TOKEN_BOOLEAN,     // true, false
  TOKEN_NUMBER,      // number 1 2 3..
  TOKEN_COMMA,       // ,
  TOKEN_EOF          // end of file.
} TokenType;

static String Token_GetTypeName(TokenType type) {
  switch (type) {
    case TOKEN_KEYWORD:
      return "Keyword";
    case TOKEN_IDENTIFIER:
      return "Identifier";
    case TOKEN_STRING:
      return "String";
    case TOKEN_LPAREN:
      return "Left Parenthesis";
    case TOKEN_RPAREN:
      return "Right Parenthesis";
    case TOKEN_LBRACE:
      return "Left Brace";
    case TOKEN_RBRACE:
      return "Right Brace";
    case TOKEN_ARROW:
      return "Arrow";
    case TOKEN_COLON:
      return "Colon";
    case TOKEN_TYPE:
      return "Type";
    case TOKEN_BOOLEAN:
      return "Boolean";
    case TOKEN_NUMBER:
      return "Number";
    case TOKEN_COMMA:
      return "Comma";
    case TOKEN_EOF:
      return "End of File";
    default:
      return "Invalid Token";
  };
}

typedef struct {
  TokenType type;
  String text;
} Token;

typedef Vector TokenVector;

Token* Token_New(TokenType type, String text);

typedef struct {
  String __input__;
  TokenVector* tokens;
  unsigned int __pos__;
} Lexer;

Lexer* Lexer_New(String);

void Lexer_Delete(Lexer*);

void Lexer_Tokenize(Lexer*);

#endif