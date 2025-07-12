#ifndef __IO_H__
#define __IO_H__

#include <kilate/native.h>
#include <kilate/node.h>

klt_node* std_print(klt_native_fndata*);

klt_node* std_system(klt_native_fndata*);

klt_node* std_sleep(klt_native_fndata*);

#endif