#include <stdio.h>
#include <string.h>

#include "kilate/config.h"
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
    printf("  -L<path>    Kilate Libraries path\n");
    printf("  -LN<path>   Kilate Native Libraries path\n");
    return true;
  }

  if (!klt_str_equals(argv[1], "run")) {
    printf("Unknown command: %s\n", argv[1]);
    return false;
  }

  // Config
  {
    const klt_str PREFIX = getenv("PREFIX");
    if (PREFIX != NULL) {
      char path[512];
      snprintf(path, sizeof(path), "%s/kilate/libs/", PREFIX);
      klt_str dup = strdup(path);
      klt_vector_push_back(libs_directories, &dup);
    }
  }
  {
    const klt_str PREFIX = getenv("PREFIX");
    if (PREFIX != NULL) {
      char path[512];
      snprintf(path, sizeof(path), "%s/kilate/native_libs/", PREFIX);
      klt_str dup = strdup(path);
      klt_vector_push_back(libs_native_directories, &dup);
    }
  }

  for (int i = 2; i < argc; i++) {
    char* arg = argv[i];
    if (arg[0] == '-') {
      if (strncmp(arg, "-LN", 3) == 0) {
        klt_str dup = strdup(&arg[3]);
        klt_vector_push_back(libs_native_directories, &dup);
      } else if (strncmp(arg, "-L", 2) == 0) {
        klt_str dup = strdup(&arg[2]);
        klt_vector_push_back(libs_directories, &dup);
      } else {
        printf("Unknown option: %s\n", arg);
        return false;
      }
    } else {
      klt_str dup = strdup(arg);
      ;
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
    klt_file_close(file);

    if (!interRes)
      return false;
  }

  return true;
}

int main(int argc, char* argv[]) {
  klt_config_init();
  klt_bool runRes = run(argc, argv);
  klt_config_end();
  return runRes ? 0 : 1;
}