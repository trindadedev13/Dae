#include "kilate/file.h"

#include <stdio.h>
#include <stdlib.h>

#include "kilate/error.h"
#include "kilate/string.h"

// Opens a file
// After do all uses with file, close it
// with klt_file_close
klt_file* klt_file_open(klt_str filePath, klt_str mode) {
  klt_file* file = malloc(sizeof(klt_file));
  if (file == NULL) {
    klt_error_fatal("Failed to open %s", filePath);
    return NULL;
  }
  file->std_file = fopen(filePath, mode);
  if (file->std_file == NULL) {
    klt_error_fatal("Failed to open %s", filePath);
    return NULL;
  }
  file->path = filePath;
  return file;
}

void klt_file_close(klt_file* file) {
  fclose(file->std_file);
  free(file);
}

// Returns the length of klt_file content.
size_t klt_file_get_length(klt_file* file) {
  fseek(file->std_file, 0, SEEK_END);
  size_t len = ftell(file->std_file);
  rewind(file->std_file);
  return len;
}

// Reads the content of klt_file.
// Result should be free.
klt_str klt_file_read_text(klt_file* file) {
  if (file == NULL || file->std_file == NULL) {
    klt_error_fatal("Failed to allocate memory to open file.");
    return NULL;
  }
  size_t fileLen = klt_file_get_length(file);
  klt_str buffer = malloc(fileLen + 1);  // 1 for null-terminate
  if (buffer == NULL) {
    klt_error_fatal("Failed to allocate memory to read file %s", file->path);
    return NULL;
  }
  fread(buffer, 1, fileLen, file->std_file);  // read file into buffer
  buffer[fileLen] = '\0';                     // null-terminate.
  return buffer;
}