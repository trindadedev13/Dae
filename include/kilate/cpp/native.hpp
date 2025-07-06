#ifdef __cplusplus

#ifndef __NATIVE_HPP__
#define __NATIVE_HPP__

#include <string>
#include <iostream>

extern "C" {
  #include "kilate/native.h"
}

namespace kilate {
  namespace native {
    /** These methods also checks the 'var' type. */
    /** Returns a string value from a param in klt_native_fndata */
    std::string getFnDataString(klt_native_fndata* data, size_t index);
    /** Returns a int value from a param in klt_native_fndata */
    int getFnDataInt(klt_native_fndata* data, size_t index, bool* ok = nullptr);
    /** Returns a float value from a param in klt_native_fndata */
    float getFnDataFloat(klt_native_fndata* data, size_t index, bool* ok = nullptr);
    /** Returns a long value from a param in klt_native_fndata */
    long getFnDataLong(klt_native_fndata* data, size_t index, bool* ok = nullptr);
    /** Returns a bool value from a param in klt_native_fndata */
    bool getFnDataBool(klt_native_fndata* data, size_t index, bool* ok = nullptr);

    /** Adds a param in params vector */
    void addParam(klt_str_vector* params, const std::string& param);
    /** Creates a klt_native_fnentry */
    klt_native_fnentry* createFnEntry(const std::string& name, klt_str_vector* reqParams, klt_native_fn fn);
  }
  namespace node {
    klt_node* createReturnNode(klt_node_valuetype return_type, void* return_value);
  }
}

#endif
#endif