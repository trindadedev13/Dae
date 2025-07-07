#ifndef __CONFIG__
#define __CONFIG__

#include "kilate/vector.h"

extern klt_vector* files;
extern klt_vector* libs_directories;
extern klt_vector* libs_native_directories;

void klt_config_init();
void klt_config_end();

#endif