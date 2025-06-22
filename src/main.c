#include <assert.h>
#include <stdio.h>

#include "dae/error.h"
#include "dae/file.h"
#include "dae/interpreter.h"
#include "dae/lexer.h"
#include "dae/parser.h"
#include "dae/string.h"

bool Interpret(String src) {
  Lexer* lexer = Lexer_New(src);
  assert(lexer);
  Lexer_Tokenize(lexer);

  Parser* parser = Parser_New(lexer->tokens);
  assert(parser);

  Parser_ParseProgram(parser);

  Interpreter* interpreter =
      Interpreter_New(parser->functions, parser->nativeFunctions);
  assert(interpreter);

  InterpreterResult result = Interpreter_Run(interpreter);

  assert(result.data);

  Interpreter_Delete(interpreter);
  Parser_Delete(parser);
  Lexer_Delete(lexer);
  return true;
}

bool Run(int argc, char* argv[]) {
  if (argc < 2) {
    printf("Usage: %s <action> <file>\n", argv[0]);
    printf("Use %s help for more info.\n", argv[0]);
    return false;
  }
  String action = argv[1];
  if (String_Equals(action, "run")) {
    // TODO: SUPPORT MULTIPLES FILES
    if (argc < 3) {
      Error_Fatal("Please provide at least 1 file!");
      return false;
    }
    File* file = File_Open(argv[2], FileMode_Read);
    if (file == NULL) {
      Error_Fatal("Failed to open %s", argv[2]);
      return false;
    }
    String src = File_ReadText(file);
    if (src == NULL) {
      Error_Fatal("Failed to read %s", argv[2]);
      return false;
    }
    bool interRes = Interpret(src);
    free(src);
    File_Close(file);
    return interRes;
  } else if (String_Equals(action, "help")) {
    printf("%s help        : prints help\n", argv[0]);
    printf("%s run <files> : executes an dae file.\n", argv[0]);
    return true;
  }
  return false;
}

int main(int argc, char* argv[]) {
  bool runRes = Run(argc, argv);
  return runRes ? 0 : 1;
}