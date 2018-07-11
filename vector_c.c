#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

#include "vector_c.h"

void vector_init(vector* v, int vectortype)
{
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->type = vectortype;
    switch (vectortype)
    {
        case INTEGER:
            v->groupItems = NULL;
            v->intItems = (int*)malloc(sizeof(int) * v->capacity);
            break;

        case GROUP:
            v->intItems = NULL;
            v->groupItems = (Group*)malloc(sizeof(Group) * v->capacity);
            break;

        default:
            break;
    }
}

bool vector_isEmpty(vector* v)
{
    return (v->total==0)?true:false;
}

int vector_type(vector* v)
{
    return v->type;
}

int vector_total(vector *v)
{
    return v->total;
}

static void vector_resize(vector* v, int capacity)
{
    #ifdef DEBUG_ON
        printf("vector_resize: %d to %d\n", v->capacity, capacity);
    #endif
    int* intItems;
    Group* groupItems;
    switch (v->type)
    {
        case INTEGER:
            intItems = realloc(v->intItems, sizeof(int) * capacity);
            if (intItems) 
            {
                v->groupItems = NULL;
                v->intItems = intItems;
                v->capacity = capacity;
            }
            break;
        case GROUP:
            groupItems = realloc(v->groupItems, sizeof(Group) * capacity);
            if (groupItems) 
            {
                v->intItems = NULL;
                v->groupItems = groupItems;
                v->capacity = capacity;
            }
            break;
        default:
            break;
    }
}

void vector_add(vector* v, void* item)
{
    int item1;
    Group g;
    if (v->capacity == v->total)
        vector_resize(v, v->capacity * 2);
    
    switch(v->type)
    {
        case INTEGER:
            item1 = *((int*)item);
            v->intItems[v->total++] = item1;
            break;

        case GROUP:
            g = ((Group)item);
            v->groupItems[v->total++] = g;
            break;

        default:
            break;
    }
}

void* vector_get(vector *v, int index)
{
    switch(v->type)
    {
        case INTEGER:
            if (index >= 0 && index < v->total)
                return &(v->intItems[index]);
            break;
        case GROUP:
            if (index >= 0 && index < v->total)
                return v->groupItems[index];
            break;
        default:
            return NULL;
            break;
    }
    
}

void vector_free(vector *v)
{
    int i;
    switch(v->type)
    {
        case INTEGER:
            free(v->intItems);
            break;
        case GROUP:
            for(i=0;i<v->total;i++)
            {
                free(v->groupItems[i]);
            }
            free(v->groupItems);
            break;
        default:
            break;
    }
}
