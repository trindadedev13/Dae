#include "kilate/environment.h"

#include <stdlib.h>
#include <string.h>

#include "kilate/bool.h"
#include "kilate/error.h"
#include "kilate/string.h"

klt_environment* klt_environment_make(klt_environment* parent) {
  klt_environment* env = malloc(sizeof(klt_environment));
  if (env == NULL)
    klt_error_fatal("Failed to allocate environment");

  env->variables = NULL;
  env->parent = parent;
  return env;
}

void klt_environment_destroy(klt_environment* env) {
  if (env == NULL)
    return;

  klt_env_entry* current = env->variables;
  while (current) {
    klt_env_entry* next = current->next;
    free(current->name);
    if (current->value) {
      klt_node_delete((klt_node*)current->value);
    }
    free(current);
    current = next;
  }

  free(env);
}

klt_bool klt_environment_definevar(klt_environment* env,
                                const klt_str name,
                                void* value) {
  if (env == NULL || name == NULL)
    klt_error_fatal("Environment or name is null.");

  klt_env_entry* current = env->variables;
  while (current) {
    if (klt_str_equals(current->name, name)) {
      return false;
    }
    current = current->next;
  }

  klt_env_entry* new_entry = malloc(sizeof(klt_env_entry));
  if (!new_entry)
    return false;

  new_entry->name = strdup(name);
  new_entry->value = value;
  new_entry->next = env->variables;

  env->variables = new_entry;
  return true;
}

klt_node* klt_environment_getvar(klt_environment* env, const klt_str name) {
  if (env == NULL || name == NULL)
    klt_error_fatal("Environment or name is null.");

  klt_environment* current_env = env;
  while (current_env) {
    klt_env_entry* current = current_env->variables;
    while (current) {
      if (strcmp(current->name, name) == 0) {
        return current->value;
      }
      current = current->next;
    }
    current_env = current_env->parent;
  }

  return NULL;
}

klt_bool klt_environment_setvar(klt_environment* env,
                             const klt_str name,
                             void* value) {
  if (env == NULL || name == NULL)
    klt_error_fatal("Environment or name is null.");

  klt_environment* current_env = env;
  while (current_env) {
    klt_env_entry* current = current_env->variables;
    while (current) {
      if (strcmp(current->name, name) == 0) {
        current->value = value;
        return true;
      }
      current = current->next;
    }
    current_env = current_env->parent;
  }

  return false;
}