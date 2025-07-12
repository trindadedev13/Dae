#ifndef __NATIVE_UTIL_H__
#define __NATIVE_UTIL_H__

#include <stddef.h>

#include "kilate/native.h"

#ifdef __cplusplus
extern "C" {
#endif

/** These methods also checks the 'var' type. */
/** Returns a string value from a param in klt_native_fndata */
klt_str klt_native_fndata_getstr(klt_native_fndata* data, size_t index);

/** Returns a int value from a param in klt_native_fndata */
int klt_native_fndata_getint(klt_native_fndata* data, size_t index, klt_bool* ok);

/** Returns a float value from a param in klt_native_fndata */
float klt_native_fndata_getfloat(klt_native_fndata* data, size_t index, klt_bool* ok);

/** Returns a long value from a param in klt_native_fndata */
long klt_native_fndata_getlong(klt_native_fndata* data, size_t index, klt_bool* ok);

/** Returns a klt_bool value from a param in klt_native_fndata */
klt_bool klt_native_fndata_getklt_bool(klt_native_fndata* data, size_t index, klt_bool* ok);

/** Adds a param in params vector */
void klt_params_add(klt_str_vector* params, const klt_str param);

/** Creates a klt_native_fnentry */
klt_native_fnentry* klt_native_fnentry_make(const klt_str name,
                                  klt_str_vector* reqParams,
                                  klt_native_fn fn);

#ifdef __cplusplus
}
#endif

#endif