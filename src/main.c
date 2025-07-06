#include <stdio.h>
#include <string.h>

#include "kilate/error.h"
#include "kilate/file.h"
#include "kilate/interpreter.h"
#include "kilate/lexer.h"
#include "kilate/native.h"
#include "kilate/parser.h"
#include "kilate/string.h"
#include "kilate/vector.h"

klt_bool interpret(klt_str src) {
  klt_lexer* lexer = klt_lexer_make(src);
  if (lexer == NULL)
    klt_error_fatal("Lexer is null.");
  klt_lexer_tokenize(lexer);

  klt_native_init();

  klt_parser* parser = klt_parser_make(lexer->tokens);
  if (parser == NULL)
    klt_error_fatal("Parser is null.");

  klt_parser_parse_program(parser);

  klt_interpreter* interpreter =
      klt_interpreter_make(parser->nodes, native_functions);
  if (interpreter == NULL)
    klt_error_fatal("Interpreter is null.");
  klt_interpreter_run(interpreter);

  klt_parser_delete(parser);
  klt_interpreter_delete(interpreter);
  klt_lexer_delete(lexer);
  klt_native_end();
  return true;
}

klt_bool run(int argc, char* argv[]) {
  if (argc < 3) {
    printf("Usage: %s run <file(s)> [-I<path>] [-l<lib>]\n", argv[0]);
    printf("Use '%s help' for more info.\n", argv[0]);
    return false;
  }

  if (klt_str_equals(argv[1], "help")) {
    printf("Usage:\n");
    printf("  %s run <file(s)> [-I<path>] [-l<lib>]\n", argv[0]);
    printf("Options:\n");
    printf("  -l<path>   Link Kilate Library path\n");
    printf("  -ln<lib>    Link Kilate NATIVE Library library\n");
    return true;
  }

  if (!klt_str_equals(argv[1], "run")) {
    printf("Unknown command: %s\n", argv[1]);
    return false;
  }

  klt_vector* files = klt_vector_make(sizeof(klt_str));
  klt_vector* libs = klt_vector_make(sizeof(klt_str));
  klt_vector* libs_native = klt_vector_make(sizeof(klt_str));

  for (int i = 2; i < argc; i++) {
    char* arg = argv[i];
    if (arg[0] == '-') {
      if (arg[1] == 'l') {
        klt_str dup = strdup(&arg[2]);
        klt_vector_push_back(libs, &dup);
      } else if (strncmp(arg, "-nl", 3) == 0) {
        klt_str dup = strdup(&arg[2]);
        klt_vector_push_back(libs_native, &dup);
      } else {
        printf("Unknown option: %s\n", arg);
        return false;
      }
    } else {
      klt_str dup = strdup(arg);
      klt_vector_push_back(files, &dup);
    }
  }

  if (files->size == 0) {
    klt_error_fatal("No input provided.");
    return false;
  }

  for (size_t i = 0; i < files->size; ++i) {
    klt_str filename = *(klt_str*)klt_vector_get(files, i);
    klt_file* file = klt_file_open(filename, FILE_MODE_READ);
    if (!file) {
      klt_error_fatal("Failed to open %s", filename);
      return false;
    }

    klt_str src = klt_file_read_text(file);
    if (!src) {
      klt_error_fatal("Failed to read %s", filename);
      return false;
    }

    if (files->size > 1)
      printf("------%s------\n", filename);

    klt_bool interRes = interpret(src);
    free(src);
    free(filename);
    klt_file_close(file);

    if (!interRes)
      return false;
  }

  for (size_t i = 0; i < libs->size; ++i) {
    klt_str lib = *(klt_str*)klt_vector_get(libs, i);
    // do something here soon
    free(lib);
  }

  for (size_t i = 0; i < libs_native->size; ++i) {
    klt_str lib = *(klt_str*)klt_vector_get(libs_native, i);
    // do something here soon
    free(lib);
  }

  klt_vector_delete(libs);
  klt_vector_delete(libs_native);
  klt_vector_delete(files);

  return true;
}

int main(int argc, char* argv[]) {
  klt_bool runRes = run(argc, argv);
  return runRes ? 0 : 1;
}