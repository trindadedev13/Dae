#include "dae/string.h"

#include <stdlib.h>

#include "dae/error.h"

size_t String_Length(String str) {
  size_t len = 0;
  while (str[len] != '\0') {
    len++;
  }
  return len;
}

bool String_StartsWith(String str, String startWith, size_t offset) {
  if (strncmp(str + offset, startWith, String_Length(startWith)) == 0) {
    return true;
  }
  return false;
}

size_t String_IndexOf(const String str, char ch, size_t offset) {
  String ptr = strchr(str + offset, ch);
  if (ptr == NULL) {
    printf("Failed to get index of string %s\n", str);
    return SIZE_MAX;
  }
  return ptr - str;
}

String String_Substring(const String str, size_t start, size_t end) {
  if (!str || start > end || end > String_Length(str)) {
    return NULL;
  }

  size_t len = end - start;
  String result = malloc(len + 1);
  if (!result)
    return NULL;

  memcpy(result, str + start, len);
  result[len] = '\0';
  return result;
}

bool String_Equals(const String str, const String other) {
  if (strcmp(str, other) == 0) {
    return true;
  }
  return false;
}

void String_Concat(String dest, const String toConcat) {
  strcat(dest, toConcat);
}

int String_ToInt(const String src) {
  int num = 0;
  int i = 0;
  int sign = 1;

  if (src[0] == '-') {
    sign = -1;
    i = 1;
  }

  for (; i < String_Length(src); ++i) {
    if (src[i] >= '0' && src[i] <= '9') {
      num = num * 10 + (src[i] - '0');
    } else {
      break;
    }
  }

  return sign * num;
}

String String_Format(const String fmt, ...) {
  va_list args;
  va_start(args, fmt);

  size_t len = vsnprintf(NULL, 0, fmt, args);
  va_end(args);

  String buffer = malloc(len + 1);
  if (!buffer)
    return NULL;

  va_start(args, fmt);
  vsnprintf(buffer, len + 1, fmt, args);
  va_end(args);

  return buffer;
}