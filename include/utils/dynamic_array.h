#ifndef DYN_ARRAY_H
#define DYN_ARRAY_H

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

//https://constantsmatter.com/posts/generic-ds-c/
#define da_MIN_CAP 64
#define da_struct(T)                                                       \
  typedef struct T##_da {                                                  \
    T *buf;                                                                    \
    size_t capacity;                                                           \
    size_t size;                                                               \
  } T##_da;

#define da_init(T)                                                         \
  static inline void T##_da_init(T##_da *vec) {                                      \
    vec->capacity = da_MIN_CAP;                                            \
    vec->buf = malloc(sizeof(T) * vec->capacity);                              \
    vec->size = 0;                                                             \
  }

#define da_get(T)                                                          \
  static inline void *T##_da_get(T##_da *vec, size_t idx) { return vec->buf + idx; }

#define da_set(T)                                                          \
  static inline void T##_da_set(T##_da *vec, size_t idx, T data) {                   \
    vec->buf[idx] = data;                                                      \
  }

#define da_push(T)                                                         \
  static inline void T##_da_push(T##_da *vec, T data) {                              \
    if (vec->size == vec->capacity) {                                          \
      vec->capacity *= 2;                                                      \
      vec->buf = realloc(vec->buf, sizeof(T) * vec->capacity);                 \
    }                                                                          \
    T##_da_set(vec, vec->size++, data);                                    \
  }

#define dynamic_array(T)                                                              \
  da_struct(T);                                                            \
  da_init(T) da_get(T) da_set(T) da_push(T)


#include "math/vector/vector2.h"
#include "math/vector/vector3.h"

// these need to be defined here before use in the declaration and definition
dynamic_array(int);
dynamic_array(float);
dynamic_array(vector2);
dynamic_array(vector3);

#endif


