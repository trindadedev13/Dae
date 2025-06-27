#include "kilate/lexer.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "kilate/error.h"
#include "kilate/string.h"

Lexer* Lexer_New(String input) {
  Lexer* lexer = malloc(sizeof(Lexer));
  lexer->__pos__ = 0;
  lexer->__input__ = strdup(input);
  lexer->tokens = Vector_New(sizeof(Token*));
  lexer->__line__ = 1;
  lexer->__column__ = 1;
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

Token* Token_New(TokenType type, String text, size_t line, size_t column) {
  Token* tk = malloc(sizeof(Token));
  tk->type = type;
  tk->text = strdup(text);
  tk->line = line;
  tk->column = column;
  return tk;
}

void Lexer_Advance(Lexer* lexer) {
  if (lexer->__input__[lexer->__pos__] == '\n') {
    lexer->__line__++;
    lexer->__column__ = 1;
  } else {
    lexer->__column__++;
  }
  lexer->__pos__++;
}

void Lexer_Tokenize(Lexer* lexer) {
  size_t inputLen = String_Length(lexer->__input__);
  while (lexer->__pos__ < inputLen) {
    char c = lexer->__input__[lexer->__pos__];
    if (isspace(c)) {
      Lexer_Advance(lexer);
      continue;
    }

    switch (c) {
      case '(': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        Token* token = Token_New(TOKEN_LPAREN, "(", tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

        Lexer_Advance(lexer);
        continue;
      }
      case ')': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        Token* token = Token_New(TOKEN_RPAREN, ")", tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

        Lexer_Advance(lexer);
        continue;
      }
      case '{': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        Token* token = Token_New(TOKEN_LBRACE, "{", tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

        Lexer_Advance(lexer);
        continue;
      }
      case '}': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        Token* token = Token_New(TOKEN_RBRACE, "}", tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

        Lexer_Advance(lexer);
        continue;
      }
      case ':': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        Token* token = Token_New(TOKEN_COLON, ":", tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

        Lexer_Advance(lexer);
        continue;
      }
      case ',': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        Token* token = Token_New(TOKEN_COMMA, ",", tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

        Lexer_Advance(lexer);
        continue;
      }
      case '=': {
        size_t tkl = lexer->__line__;
        size_t tkc = lexer->__column__;
        Token* token = Token_New(TOKEN_ASSIGN, "=", tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

        Lexer_Advance(lexer);
        continue;
      }
      case '\n': {
        // lexer->__line__++;
        //   lexer->__column__ = 1;
        Lexer_Advance(lexer);
        continue;
      }
      case ';': {
        Lexer_Advance(lexer);
        continue;
      }
    };
    if (String_StartsWith(lexer->__input__, "//", lexer->__pos__)) {
      lexer->__pos__ += 2;
      while (lexer->__pos__ < inputLen &&
             lexer->__input__[lexer->__pos__] != '\n') {
        Lexer_Advance(lexer);
      }
      continue;
    }
    if (String_StartsWith(lexer->__input__, "->", lexer->__pos__)) {
      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      Token* token = Token_New(TOKEN_ARROW, "->", tkl, tkc);
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
      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      Token* token = Token_New(TOKEN_STRING, str, tkl, tkc);
      Vector_PushBack(lexer->tokens, &token);
      free(str);

      lexer->__pos__ = end + 1;
      continue;
    }
    if (isdigit(c)) {
      size_t start = lexer->__pos__;
      while (lexer->__pos__ < inputLen &&
             isdigit(lexer->__input__[lexer->__pos__])) {
        Lexer_Advance(lexer);
        lexer->__line__++;
      }
      String number = String_Substring(lexer->__input__, start, lexer->__pos__);
      if (number == NULL) {
        Lexer_Error(lexer, "Failed to extract number");
      }
      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      Token* token = Token_New(TOKEN_NUMBER, number, tkl, tkc);
      Vector_PushBack(lexer->tokens, &token);
      free(number);
      continue;
    }

    if (isalpha(c)) {
      size_t start = lexer->__pos__;
      while (lexer->__pos__ < inputLen &&
             (isalpha(lexer->__input__[lexer->__pos__]) ||
              isdigit(lexer->__input__[lexer->__pos__]))) {
        Lexer_Advance(lexer);
      }
      String word = String_Substring(lexer->__input__, start, lexer->__pos__);
      if (word == NULL) {
        Lexer_Error(lexer, "Failed to get word");
        break;
      }
      size_t tkl = lexer->__line__;
      size_t tkc = lexer->__column__;
      if (String_Equals(word, "work") || String_Equals(word, "return")) {
        Token* token = Token_New(TOKEN_KEYWORD, word, tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

      } else if (String_Equals(word, "true") || String_Equals(word, "false")) {
        Token* token = Token_New(TOKEN_BOOL, word, tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);

      } else if (String_Equals(word, "bool") || String_Equals(word, "number") ||
                 String_Equals(word, "string") || String_Equals(word, "any")) {
        Token* token = Token_New(TOKEN_TYPE, word, tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);
      } else if (String_Equals(word, "var")) {
        Token* token = Token_New(TOKEN_VAR, word, tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);
      } else if (String_Equals(word, "let")) {
        Token* token = Token_New(TOKEN_LET, word, tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);
      } else {
        Token* token = Token_New(TOKEN_IDENTIFIER, word, tkl, tkc);
        Vector_PushBack(lexer->tokens, &token);
      }
      free(word);
      continue;
    }
    Lexer_Error(lexer, "Unexpected character %c", c);
    break;
  }
  size_t tkl = lexer->__line__;
  size_t tkc = lexer->__column__;
  Token* token = Token_New(TOKEN_EOF, "", tkl, tkc);
  Vector_PushBack(lexer->tokens, &token);
}

void Lexer_Error(Lexer* lexer, String fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[Error at %zu:%zu] ", lexer->__line__, lexer->__column__);
  vprintf(fmt, args);
  printf("\n");
  va_end(args);
  exit(1);
}