#include <stdio.h>
#include <stdlib.h>

#include "kilate/config.h"
#include "kilate/string.h"
#include "kilate/vector.h"

klt_vector* files = NULL;
klt_vector* libs_directories = NULL;
klt_vector* libs_native_directories = NULL;

void klt_config_init() {
  files = klt_vector_make(sizeof(klt_str));
  libs_directories = klt_vector_make(sizeof(klt_str));
  libs_native_directories = klt_vector_make(sizeof(klt_str));
}

void klt_config_end() {
  for (size_t i = 0; i < files->size; ++i) {
    klt_str filename = *(klt_str*)klt_vector_get(files, i);
    free(filename);
  }

  for (size_t i = 0; i < libs_directories->size; ++i) {
    klt_str lib = *(klt_str*)klt_vector_get(libs_directories, i);
    free(lib);
  }

  for (size_t i = 0; i < libs_native_directories->size; ++i) {
    klt_str lib = *(klt_str*)klt_vector_get(libs_native_directories, i);
    free(lib);
  }

  klt_vector_delete(libs_directories);
  klt_vector_delete(libs_native_directories);
  klt_vector_delete(files);
}