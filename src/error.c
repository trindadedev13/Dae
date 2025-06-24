#include "kilate/error.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "kilate/string.h"

void Error_Fatal(String fmt, ...) {
  va_list args;
  va_start(args, fmt);
  fprintf(stderr, "[ERROR] ");
  vprintf(fmt, args);
  printf("\n");
  va_end(args);
  exit(1);
}