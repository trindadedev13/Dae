#include "dae/file.h"

#include <stdio.h>
#include <stdlib.h>

#include "dae/error.h"
#include "dae/string.h"

// Opens a file
// After do all uses with file, close it
// with File_Close
File* File_Open(String filePath, String mode) {
  File* file = malloc(sizeof(File));
  if (file == NULL) {
    Error_Fatal("Failed to allocate memory to open %s\n", filePath);
    return NULL;
  }
  file->stdFile = fopen(filePath, mode);
  if (file->stdFile == NULL) {
    Error_Fatal("Failed to allocate memory to open %s\n", filePath);
    return NULL;
  }
  file->path = filePath;
  return file;
}

void File_Close(File* file) {
  fclose(file->stdFile);
  free(file);
}

// Returns the length of File content.
size_t File_GetLength(File* file) {
  fseek(file->stdFile, 0, SEEK_END);
  size_t len = ftell(file->stdFile);
  rewind(file->stdFile);
  return len;
}

// Reads the content of File.
// Result should be free.
String File_ReadText(File* file) {
  if (file == NULL || file->stdFile == NULL) {
    Error_Fatal("Failed to allocate memory to open file.");
    return NULL;
  }
  size_t fileLen = File_GetLength(file);
  String buffer = malloc(fileLen + 1);  // 1 for null-terminate
  if (buffer == NULL) {
    Error_Fatal("Failed to allocate memory to read file %s\n", file->path);
    return NULL;
  }
  fread(buffer, 1, fileLen, file->stdFile);  // read file into buffer
  buffer[fileLen] = '\0';                    // null-terminate.
  return buffer;
}