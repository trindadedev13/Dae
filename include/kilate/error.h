#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdarg.h>

#include "kilate/string.h"

#include <stdio.h>
#include <stdlib.h>

#define klt_error_fatal(fmt, ...)                                          \
  do {                                                                     \
    fprintf(stderr, "[ERROR at %s:%s:%d] ", __FILE__, __func__, __LINE__); \
    fprintf(stderr, fmt, ##__VA_ARGS__);                                   \
    fprintf(stderr, "\n");                                                 \
    exit(1);                                                               \
  } while (0)

#endif