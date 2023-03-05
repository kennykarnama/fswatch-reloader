#ifndef ARRAYSTRING_H_
#define ARRAYSTRING_H_

typedef struct {
  char **array;
  size_t used;
  size_t size;
} ArrayString;

void initArray(ArrayString *a, size_t initialSize) {
  a->array = malloc(initialSize * sizeof(char*));
  a->used = 0;
  a->size = initialSize;
}

void insertArray(ArrayString *a, char *element) {
  // a->used is the number of used entries, because a->array[a->used++] updates a->used only *after* the array has been accessed.
  // Therefore a->used can go up to a->size 
  if (a->used == a->size) {
    a->size *= 2;
    a->array = realloc(a->array, a->size * sizeof(char*));
  }
  a->array[a->used++] = element;
}

void freeArray(ArrayString *a) {
  free(a->array);
  a->array = NULL;
  a->used = a->size = 0;
}

#endif