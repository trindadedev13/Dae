#ifndef LEXER_H
#define LEXER_H

#include "kilate/inline.h"
#include "kilate/string.h"
#include "kilate/vector.h"

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
  TOKEN_BOOL,        // true, false
  TOKEN_NUMBER,      // number 1 2 3..
  TOKEN_COMMA,       // ,,
  TOKEN_ASSIGN,      // =
  TOKEN_VAR,         // var
  TOKEN_LET,         // let
  TOKEN_EOF          // end of file.
} TokenType;

static String Token_GetTypeName(TokenType type) {
  switch (type) {
    case TOKEN_KEYWORD:
      return "keyword";
    case TOKEN_IDENTIFIER:
      return "identifier";
    case TOKEN_STRING:
      return "string";
    case TOKEN_LPAREN:
      return "left_parenthesis";
    case TOKEN_RPAREN:
      return "right_parenthesis";
    case TOKEN_LBRACE:
      return "left_brace";
    case TOKEN_RBRACE:
      return "right_brace";
    case TOKEN_ARROW:
      return "arrow";
    case TOKEN_COLON:
      return "colon";
    case TOKEN_TYPE:
      return "type";
    case TOKEN_BOOL:
      return "boolean";
    case TOKEN_NUMBER:
      return "number";
    case TOKEN_COMMA:
      return "comma";
    case TOKEN_ASSIGN:
      return "assign";
    case TOKEN_LET:
      return "let";
    case TOKEN_VAR:
      return "var";
    case TOKEN_EOF:
      return "end_of_file";
    default:
      return "unknow_token";
  };
}

typedef struct {
  TokenType type;
  String text;

  size_t column;
  size_t line;
} Token;

typedef Vector TokenVector;

Token* Token_New(TokenType, String, size_t, size_t);

typedef struct {
  String __input__;
  TokenVector* tokens;

  size_t __pos__;
  size_t __column__;
  size_t __line__;
} Lexer;

Lexer* Lexer_New(String);

void Lexer_Delete(Lexer*);

void Lexer_Tokenize(Lexer*);

#endif