#ifndef FILE_H
#define FILE_H

#include <stdio.h>

#include "dae/string.h"

#define FileMode_Read "r"
#define FileMode_Write "w"

typedef struct {
  FILE* stdFile;
  String path;
} File;

File* File_Open(String, String);

void File_Close(File*);

size_t File_GetLength(File*);

String File_ReadText(File*);

#endif