/* some geometric routines always needed */

#include <math.h>

float length(float *v) {
	return (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
}

float length2(float *v) {
  return (float)sqrt(v[0] * v[0] + v[1] * v[1]);
}

float length3(float *v) {
  return length(v);
}

void normalize(float *v) {
	float d = length(v);
	if (d == 0) return;
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
}

void crossprod(float *v1, float *v2, float *out) {
	out[0] = v1[1] * v2[2] - v1[2] * v2[1];
	out[1] = v1[2] * v2[0] - v1[0] * v2[2];
	out[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

void normcrossprod(float *v1, float *v2, float *out) {
	crossprod(v1, v2, out);
	normalize(out);
}

float scalarprod2(float *v1, float *v2) {
	return v1[0] * v2[0] + v1[1] * v2[1];
}

float scalarprod(float *v1, float *v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void vsub2(float *v1, float *v2, float *out) {
	out[0] = v1[0] - v2[0];
	out[1] = v1[1] - v2[1];
}

void vsub(float *v1, float *v2, float *out) {
	out[0] = v1[0] - v2[0];
	out[1] = v1[1] - v2[1];
	out[2] = v1[2] - v2[2];
}

void vadd2(float *v1, float *v2, float *out) {
	out[0] = v1[0] + v2[0];
	out[1] = v1[1] + v2[1];
}

void vadd(float *v1, float *v2, float *out) {
	out[0] = v1[0] + v2[0];
	out[1] = v1[1] + v2[1];
	out[2] = v1[2] + v2[2];
}

void vcopy(float *v1, float *out) {
	out[0] = v1[0];
	out[1] = v1[1];
	out[2] = v1[2];
}

void vmul(float *v, float f) {
	v[0] *= f;
	v[1] *= f;
	v[2] *= f;
}

/* 4 entries... */

float length4(float *v) {
	return (float)sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2] +
		    v[3] * v[3]);
}

void normalize4(float *v) {
	float d = length(v);
	if (d == 0) return;
	v[0] /= d;
	v[1] /= d;
	v[2] /= d;
	v[3] /= d;
}

float scalarprod4(float *v1, float *v2) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2] +
	  v1[3] * v2[3];
}

void vsub4(float *v1, float *v2, float *out) {
	out[0] = v1[0] - v2[0];
	out[1] = v1[1] - v2[1];
	out[2] = v1[2] - v2[2];
	out[3] = v1[3] - v2[3];
}

void vadd4(float *v1, float *v2, float *out) {
	out[0] = v1[0] + v2[0];
	out[1] = v1[1] + v2[1];
	out[2] = v1[2] + v2[2];
	out[3] = v1[3] + v2[3];
}

void vcopy4(float *v1, float *out) {
	out[0] = v1[0];
	out[1] = v1[1];
	out[2] = v1[2];
	out[3] = v1[3];
}
