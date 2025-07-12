#ifndef __FILE_H__
#define __FILE_H__

#include <stdio.h>

#include "kilate/string.h"

#define FILE_MODE_READ "r"
#define FILE_MODE_WRITE "w"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  FILE* std_file;
  klt_str path;
} klt_file;

klt_file* klt_file_open(klt_str, klt_str);

void klt_file_close(klt_file*);

size_t klt_file_get_length(klt_file*);

klt_str klt_file_read_text(klt_file*);

#ifdef __cplusplus
}
#endif

#endif