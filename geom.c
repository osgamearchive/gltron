/* some geometric routines always needed */

#include <math.h>

float length(float v[3]) {
	return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

void normalize(float v[3]) {
	float d = length(v);
	if (d == 0) return;
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

void crossprod(float v1[3], float v2[3], float out[3]) {
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void normcrossprod(float v1[3], float v2[3], float out[3]) {
	crossprod(v1, v2, out);
	normalize(out);
}

float scalarprod(float v1[3], float v2[3]) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vsub(float v1[3], float v2[3], float out[3]) {
	out[0] = v1[0] - v2[0];
	out[1] = v1[1] - v2[1];
	out[2] = v1[2] - v2[2];
}

void vadd(float v1[3], float v2[3], float out[3]) {
	out[0] = v1[0] + v2[0];
	out[1] = v1[1] + v2[1];
	out[2] = v1[2] + v2[2];
}

void vcopy(float v1[3], float out[3]) {
	out[0] = v1[0];
	out[1] = v1[1];
	out[2] = v1[2];
}
