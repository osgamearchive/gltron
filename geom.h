/* some geometric routines always needed */

#ifndef GEOM_H
#define GEOM_H

float length(float *v);
void normalize(float *v);
void crossprod(float *v1, float *v2, float *out);
void normcrossprod(float *v1, float *v2, float *out);
float scalarprod(float *v1, float *v2);
void vsub(float *v1, float *v2, float *out);
void vadd(float *v1, float *v2, float *out);
void vcopy(float *v1, float *out);
void vmul(float *v, float f);

float length4(float *v);
void normalize4(float *v);
float scalarprod4(float *v1, float *v2);
void vsub4(float *v1, float *v2, float *out);
void vadd4(float *v1, float *v2, float *out);
void vcopy4(float *v1, float *out);
#endif




