#ifndef NEBU_UTIL_H
#define NEBU_UTIL_H

/* small utility datastructures & functions */

typedef struct nebu_List nebu_List;
struct nebu_List {
  void *data;
  nebu_List* next;
};

void nebu_RandomPermutation( int N, int *nodes );
void nebu_Clamp( float *f, float min, float max );

void nebu_List_AddTail(nebu_List **l, void* data);

#endif
