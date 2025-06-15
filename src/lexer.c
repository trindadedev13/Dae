#include "dae/lexer.h"

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dae/error.h"
#include "dae/string.h"

Lexer* Lexer_New(String input) {
  Lexer* lexer = malloc(sizeof(Lexer));
  lexer->__pos__ = 0;
  lexer->__input__ = strdup(input);
  lexer->tokens = Vector_New(sizeof(Token*));
  return lexer;
}

void Lexer_Delete(Lexer* lexer) {
  for (size_t i = 0; i < lexer->tokens->size; ++i) {
    Token* token = *(Token**)Vector_Get(lexer->tokens, i);
    free(token->text);
    free(token);
  }
  Vector_Delete(lexer->tokens);
  free(lexer->__input__);
  free(lexer);
}

Token* Token_New(TokenType type, String text) {
  Token* tk = malloc(sizeof(Token));
  tk->type = type;
  tk->text = strdup(text);
  return tk;
}

void Lexer_Tokenize(Lexer* lexer) {
  size_t inputLen = String_Length(lexer->__input__);
  while (lexer->__pos__ < inputLen) {
    char c = lexer->__input__[lexer->__pos__];
    if (isspace(c)) {
      lexer->__pos__++;
      continue;
    }
    switch (c) {
      case '(': {
        Token* token = Token_New(TOKEN_LPAREN, "(");
        Vector_PushBack(lexer->tokens, &token);

        lexer->__pos__++;
        continue;
      }
      case ')': {
        Token* token = Token_New(TOKEN_RPAREN, ")");
        Vector_PushBack(lexer->tokens, &token);

        lexer->__pos__++;
        continue;
      }
      case '{': {
        Token* token = Token_New(TOKEN_LBRACE, "{");
        Vector_PushBack(lexer->tokens, &token);

        lexer->__pos__++;
        continue;
      }
      case '}': {
        Token* token = Token_New(TOKEN_RBRACE, "}");
        Vector_PushBack(lexer->tokens, &token);

        lexer->__pos__++;
        continue;
      }
      case ':': {
        Token* token = Token_New(TOKEN_COLON, ":");
        Vector_PushBack(lexer->tokens, &token);

        lexer->__pos__++;
        continue;
      }
      case ',': {
        Token* token = Token_New(TOKEN_COMMA, ",");
        Vector_PushBack(lexer->tokens, &token);

        lexer->__pos__++;
        continue;
      }
    };
    if (String_StartsWith(lexer->__input__, "->", lexer->__pos__)) {
      Token* token = Token_New(TOKEN_ARROW, "->");
      Vector_PushBack(lexer->tokens, &token);

      lexer->__pos__ += 2;
      continue;
    }
    if (c == '"') {
      size_t end = String_IndexOf(lexer->__input__, '"', lexer->__pos__ + 1);
      if (end == SIZE_MAX) {
        printf("Unclosed string\n");
        lexer->__pos__ = inputLen;
        break;
      }
      String str = String_Substring(lexer->__input__, lexer->__pos__ + 1, end);
      if (str == NULL) {
        printf("Failed to get substr\n");
        break;
      }
      Token* token = Token_New(TOKEN_STRING, str);
      Vector_PushBack(lexer->tokens, &token);
      free(str);

      lexer->__pos__ = end + 1;
      continue;
    }
    if (isdigit(c)) {
      size_t start = lexer->__pos__;
      while (lexer->__pos__ < inputLen &&
             isdigit(lexer->__input__[lexer->__pos__])) {
        lexer->__pos__++;
      }
      String number = String_Substring(lexer->__input__, start, lexer->__pos__);
      if (number == NULL) {
        Error_Fatal("Failed to extract number");
      }
      Token* token = Token_New(TOKEN_NUMBER, number);
      Vector_PushBack(lexer->tokens, &token);
      free(number);

      continue;
    }

    if (isalpha(c)) {
      size_t start = lexer->__pos__;
      while (lexer->__pos__ < inputLen &&
             (isalpha(lexer->__input__[lexer->__pos__]) ||
              isdigit(lexer->__input__[lexer->__pos__])))
        lexer->__pos__++;
      String word = String_Substring(lexer->__input__, start, lexer->__pos__);
      if (word == NULL) {
        Error_Fatal("Failed to get word");
        break;
      }

      if (String_Equals(word, "work") || String_Equals(word, "return")) {
        Token* token = Token_New(TOKEN_KEYWORD, word);
        Vector_PushBack(lexer->tokens, &token);

      } else if (String_Equals(word, "true") || String_Equals(word, "false")) {
        Token* token = Token_New(TOKEN_BOOLEAN, word);
        Vector_PushBack(lexer->tokens, &token);

      } else if (String_Equals(word, "bool") || String_Equals(word, "int") ||
                 String_Equals(word, "string")) {
        Token* token = Token_New(TOKEN_TYPE, word);
        Vector_PushBack(lexer->tokens, &token);

      } else if (word != NULL) {
        Token* token = Token_New(TOKEN_IDENTIFIER, word);
        Vector_PushBack(lexer->tokens, &token);
      }
      free(word);
      continue;
    }
    printf("Unexpected character %c\n", c);
  }
  Token* token = Token_New(TOKEN_EOF, "");
  Vector_PushBack(lexer->tokens, &token);
}