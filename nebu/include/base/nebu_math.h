#ifndef NEBU_MATH_H
#define NEBU_MATH_H

#include <math.h>
#include <limits.h>

#ifndef M_PI
#define M_PI 3.141592654
#endif

#define PI ((float) M_PI)

#ifdef WIN32
#define floorf (float)floor
#define fabsf (float)fabs
#define sqrtf (float)sqrt
#define cosf (float) cos
#define sinf (float) sin
#define acosf (float) acos
#define tanf (float) tan
#endif

#define nebu_cosf_deg(X)	cosf( (X) * M_PI/180.0 )
#define nebu_sinf_deg(X)	sinf( (X) * M_PI/180.0 )

#ifndef FLT_MAX
#define FLT_MAX 3.402823466e+38F
#endif

#ifndef FLT_MIN
#define FLT_MIN 1.175494351e-38F
#endif

#endif
