/* some geometric routines always needed */

#ifndef GEOM_H
#define GEOM_H

float length(float v[3]);
void normalize(float v[3]);
void crossprod(float v1[3], float v2[3], float out[3]);
void normcrossprod(float v1[3], float v2[3], float out[3]);
float scalarprod(float v1[3], float v2[3]);
void vsub(float v1[3], float v2[3], float out[3]);
void vadd(float v1[3], float v2[3], float out[3]);
void vcopy(float v1[3], float out[3]);
#endif
