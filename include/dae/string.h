#ifndef STRING_H
#define STRING_H

#include <stdarg.h>
#include <stdlib.h>

typedef char* String;

size_t String_Length(const String);

bool String_StartsWith(const String, const String, size_t);

size_t String_IndexOf(const String, char, size_t);

String String_Substring(const String, size_t, size_t);

bool String_Equals(const String, const String);

void String_Concat(String, String);

int String_ToInt(String);

String String_Format(const String, ...);

#endif