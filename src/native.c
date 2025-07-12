#include "kilate/native.h"

#include <dirent.h>
#include <dlfcn.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kilate/config.h"
#include "kilate/lexer.h"
#include "kilate/node.h"
#include "kilate/string.h"
#include "kilate/vector.h"

klt_node_vector* native_functions = NULL;

void klt_native_init() {
  native_functions = klt_vector_make(sizeof(klt_native_fnentry*));
  klt_native_load_extern();
}

void klt_native_load_extern() {
  // Load ALL Native Libs found
  for (size_t i = 0; i < libs_native_directories->size; i++) {
    klt_str dir = *(klt_str*)klt_vector_get(libs_native_directories, i);
    DIR* d = opendir(dir);
    if (!d)
      return;

    struct dirent* entry;
    while ((entry = readdir(d))) {
      if (strstr(entry->d_name, ".so")) {
        char path[512];
        snprintf(path, sizeof(path), "%s%s", dir, entry->d_name);

        void* handle = dlopen(path, RTLD_NOW);
        if (!handle) {
          fprintf(stderr, "Error loading %s: %s\n", path, dlerror());
          continue;
        }

        void (*extern_native_init)() = dlsym(handle, "KILATE_NATIVE_REGISTER");
        if (!extern_native_init) {
          fprintf(stderr, "Function KILATE_NATIVE_REGISTER not found in %s\n",
                  path);
          continue;
        }
        extern_native_init();
      }
    }
    closedir(d);
  }
}

void klt_native_end() {
  for (size_t i = 0; i < native_functions->size; ++i) {
    klt_native_fnentry* entry =
        *(klt_native_fnentry**)klt_vector_get(native_functions, i);
    free(entry->name);
    if (entry->requiredParams != NULL)
      klt_vector_delete(entry->requiredParams);
    free(entry);
  }
  klt_vector_delete(native_functions);
}

void klt_native_register_fnentry(klt_native_fnentry* entry) {
  klt_vector_push_back(native_functions, &entry);
}

void klt_native_register_fn(klt_str name,
                            klt_str_vector* requiredParams,
                            klt_native_fn fn) {
  klt_native_fnentry* entry = malloc(sizeof(klt_native_fnentry));
  entry->name = strdup(name);
  entry->fn = fn;
  entry->requiredParams = requiredParams;
  klt_native_register_fnentry(entry);
}

klt_native_fnentry* klt_native_find_function(klt_str name) {
  for (size_t i = 0; i < native_functions->size; ++i) {
    klt_native_fnentry* entry =
        *(klt_native_fnentry**)klt_vector_get(native_functions, i);
    if (klt_str_equals(entry->name, name)) {
      return entry;
    }
  }
  return NULL;
}