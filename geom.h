/* some geometric routines always needed */

#ifndef GEOM_H
#define GEOM_H


float length2(float *v);
float length3(float *v);
float length4(float *v);
float length(float *v);
void normalize(float *v);
void normalize4(float *v);
void crossprod(float *v1, float *v2, float *out);
void normcrossprod(float *v1, float *v2, float *out);
float scalarprod(float *v1, float *v2);
float scalarprod2(float *v1, float *v2);
float scalarprod4(float *v1, float *v2);
void vsub(float *v1, float *v2, float *out);
void vsub2(float *v1, float *v2, float *out);
void vsub4(float *v1, float *v2, float *out);
void vadd(float *v1, float *v2, float *out);
void vadd2(float *v1, float *v2, float *out);
void vadd4(float *v1, float *v2, float *out);
void vcopy(float *v1, float *out);
void vcopy4(float *v1, float *out);
void vmul(float *v, float f);

#endif




