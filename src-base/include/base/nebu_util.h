#ifndef NEBU_UTIL_H
#define NEBU_UTIL_H

/* small utility macros & functions */

#include "base/nebu_types.h"
#include <math.h>

#define COS(X)	cos( (X) * M_PI/180.0 )
#define SIN(X)	sin( (X) * M_PI/180.0 )

#ifndef M_PI
#define M_PI 3.141592654
#endif

void randomPermutation( int N, int *nodes );
void clamp( float *f, float min, float max );
void addList(List **l, void* data);

#endif
