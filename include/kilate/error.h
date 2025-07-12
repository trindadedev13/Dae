#ifndef __ERROR_H__
#define __ERROR_H__

#include <stdarg.h>

#include "kilate/string.h"

#ifdef __cplusplus
extern "C" {
#endif

void klt_error_fatal(klt_str, ...);

#ifdef __cplusplus
}
#endif

#endif