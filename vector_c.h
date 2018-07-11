#ifndef VECTOR_H
#define VECTOR_H

#define VECTOR_INIT_CAPACITY 4
#include "Def.h"


//vec is of type vector*
//type signifies whether it is an integer vector or Group* vector

void vector_init(vector *, int type);
int vector_total(vector *);
static void vector_resize(vector *, int);
void vector_add(vector *, void*);
void vector_set(vector *, int, int);
void *vector_get(vector *, int);
void vector_delete(vector *, int);
void vector_free(vector *);
bool vector_isEmpty(vector* v);
int vector_type(vector* v);

#define VECTOR_INIT(vec, type) vector_init(vec, (int)type)
#define VECTOR_ADD(vec, item) vector_add(vec, (void*) item)
#define VECTOR_SET(vec, id, item) vector_set(vec, id, (void *) item)
#define VECTOR_GET(vec, type, id) (type) vector_get(vec, id)
#define VECTOR_DELETE(vec, id) vector_delete(vec, id)
#define VECTOR_TOTAL(vec) vector_total(vec)
#define VECTOR_FREE(vec) vector_free(vec)
#define VECTOR_ISEMPTY(vec) vector_isEmpty(vec)
#define VECTOR_TYPE(vec) vector_type(vec)



#endif
