#ifndef UTIL_H
#define UTIL_H

/* small utility macros & functions */

#include "basic_types.h"

#define COS(X)	cos( (X) * M_PI/180.0 )
#define SIN(X)	sin( (X) * M_PI/180.0 )

void randomPermutation( int N, int *nodes );
void clamp( float *f, float min, float max );
void addList(List **l, void* data);
void runScript(int ePath, const char *name);

#endif
