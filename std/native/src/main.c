#include <kilate/native.h>
#include <kilate/string.h>
#include <kilate/vector.h>

#include "io.h"

KILATE_NATIVE_REGISTER() {
  {
    // Register native print method
    klt_str_vector* requiredParams = klt_vector_make(sizeof(klt_str*));
    klt_str any = "any";
    klt_vector_push_back(requiredParams, &any);
    klt_native_register_fn("print", requiredParams, std_print);
  }
  {
    // Register native system method
    klt_str_vector* requiredParams = klt_vector_make(sizeof(klt_str*));
    klt_str str = "string";
    klt_vector_push_back(requiredParams, &str);
    klt_native_register_fn("system", requiredParams, std_system);
  }
  {
    // Register native system method
    klt_str_vector* requiredParams = klt_vector_make(sizeof(klt_str*));
    klt_str str = "long";
    klt_vector_push_back(requiredParams, &str);
    klt_native_register_fn("sleep", requiredParams, std_sleep);
  }
}