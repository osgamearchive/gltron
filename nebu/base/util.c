/* small utility functions */

#include <stdlib.h>
#include "base/nebu_random.h"
#include "base/nebu_util.h"

void nebu_RandomPermutation( int N, int *nodes )
{
  int i;
  for(i = 0; i < N; i++)
    nodes[i] = i;

  for(i = 0; i < N - 1; i++) {
    int s, t;
    int tmp;
    t = N - 1 - i;
    // s = (int) ((float)( t + 1 ) * nebu_rand() / (RAND_MAX + 1.0f));
    s = nebu_rand() % (t + 1);
    tmp = nodes[t];
    nodes[t] = nodes[s];
    nodes[s] = tmp;
  }
}

void nebu_Clamp( float *f, float min, float max )
{
  if(*f < min) *f = min;
  else if(*f > max) *f = max;
}

void nebu_List_AddTail(nebu_List **l, void* data) {
	nebu_List *p;
	if(*l == NULL) {
		*l = (nebu_List*) malloc(sizeof(nebu_List));
		(*l)->next = NULL;
	}
	for(p = *l; p->next != NULL; p = p->next);
	p->next = (nebu_List*) malloc(sizeof(nebu_List));
	p->next->next = NULL;
	p->data = data;
}
