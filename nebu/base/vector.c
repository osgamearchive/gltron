#include "base/nebu_vector.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

vec4* vec4Add(vec4 *pOut, const vec4 *pV1, const vec4 *pV2) {
  pOut->v[0] = pV1->v[0] + pV2->v[0];
  pOut->v[1] = pV1->v[1] + pV2->v[1];
  pOut->v[2] = pV1->v[2] + pV2->v[2];
  pOut->v[3] = pV1->v[3] + pV2->v[3];
  return pOut;
}

vec4* vec4Sub(vec4 *pOut, const vec4 *pV1, const vec4 *pV2) {
  pOut->v[0] = pV1->v[0] - pV2->v[0];
  pOut->v[1] = pV1->v[1] - pV2->v[1];
  pOut->v[2] = pV1->v[2] - pV2->v[2];
  pOut->v[3] = pV1->v[3] - pV2->v[3];
  return pOut;
}

vec3* vec3Add(vec3 *pOut, const vec3 *pV1, const vec3 *pV2) {
  pOut->v[0] = pV1->v[0] + pV2->v[0];
  pOut->v[1] = pV1->v[1] + pV2->v[1];
  pOut->v[2] = pV1->v[2] + pV2->v[2];
  return pOut;
}

vec3* vec3Sub(vec3 *pOut, const vec3 *pV1, const vec3 *pV2) {
  pOut->v[0] = pV1->v[0] - pV2->v[0];
  pOut->v[1] = pV1->v[1] - pV2->v[1];
  pOut->v[2] = pV1->v[2] - pV2->v[2];
  return pOut;
}

vec3* vec3Cross(vec3 *pOut, const vec3 *pV1, const vec3 *pV2) {
  vec3 tmp;
  tmp.v[0] = pV1->v[1] * pV2->v[2] - pV1->v[2] * pV2->v[1];
  tmp.v[1] = pV1->v[2] * pV2->v[0] - pV1->v[0] * pV2->v[2];
  tmp.v[2] = pV1->v[0] * pV2->v[1] - pV1->v[1] * pV2->v[0];
  memcpy(pOut, &tmp, sizeof(vec3));
  return pOut;
}

float vec3Dot(const vec3 *pV1, const vec3 *pV2) {
  return pV1->v[0] * pV2->v[0] + 
    pV1->v[1] * pV2->v[1] + pV1->v[2] * pV2->v[2];
}

float vec4Dot(const vec4 *pV1, const vec4 *pV2) {
  return 
    pV1->v[0] * pV2->v[0] + pV1->v[1] * pV2->v[1] + 
    pV1->v[2] * pV2->v[2] + pV1->v[3] * pV2->v[3];
}

float vec3Length(const vec3 *pV) {
  return (float) sqrt( pV->v[0] * pV->v[0] + 
	       pV->v[1] * pV->v[1] + pV->v[2] * pV->v[2] );
}

float vec3LengthSqr(const vec3 *pV) {
  return pV->v[0] * pV->v[0] + 
    pV->v[1] * pV->v[1] + pV->v[2] * pV->v[2];
}
    
vec3* vec3Normalize(vec3 *pOut, const vec3 *pV) {
  float fLength = vec3Length(pV);
  if(fLength != 0) {
    pOut->v[0] = pV->v[0] / fLength;
    pOut->v[1] = pV->v[1] / fLength;
    pOut->v[2] = pV->v[2] / fLength;
  }
  return pOut;
}

void vec4Print(const vec4 *v) {
  int i;
  printf("[ ");
  for(i = 0; i < 4; i++) {
    printf("%.3f ", v->v[i]);
  }
  printf(" ]\n");
}

void vec3Print(const vec3 *v) {
  int i;
  printf("[ ");
  for(i = 0; i < 3; i++) {
    printf("%.3f ", v->v[i]);
  }
  printf(" ]\n");
}

vec4* vec4fromVec3(vec4 *pOut, const vec3 *pV) {
  memcpy(pOut, pV, sizeof(vec3));
  pOut->v[3] = 1;
  return pOut;
}

vec3* vec3fromVec4(vec3 *pOut, const vec4 *pV) {
  if(pV->v[3] != 0) {
    pOut->v[0] = pV->v[0] / pV->v[3];
    pOut->v[1] = pV->v[1] / pV->v[3];
    pOut->v[2] = pV->v[2] / pV->v[3];
    return pOut;
  } else {
    memset(pOut, 0, sizeof(vec3));
    return NULL;
  }
}

vec3* vec3Copy(vec3 *pOut, const vec3 *pV) {
	memcpy(pOut, pV, sizeof(vec3));
	return pOut;
}

vec3* vec3Scale(vec3 *pOut, const vec3 *pV, float f) {
	pOut->v[0] = f * pV->v[0];
	pOut->v[1] = f * pV->v[1];
	pOut->v[2] = f * pV->v[2];
	return pOut;
}

unsigned int uintFromVec3(vec3 *pV) {
	return
		( ( (unsigned int)(pV->v[0] * 127.0f + 128.0f) ) << 16 ) +
		( ( (unsigned int)(pV->v[1] * 127.0f + 128.0f) ) << 8 ) +
		( ( (unsigned int)(pV->v[2] * 127.0f + 128.0f) ) << 0 );
}
		
vec3* vec3Zero(vec3 *pV) {
	memset(pV, 0, sizeof(vec3));
	return pV;
}

vec3* vec3TriNormalDirection(vec3* pOut, 
														 const vec3* pV1, 
														 const vec3* pV2, 
														 const vec3 *pV3) {
	vec3 v1, v2;
	vec3Sub(&v1, pV2, pV1);
	vec3Sub(&v2, pV3, pV1);
	vec3Cross(pOut, &v1, &v2);
	return pOut;
}

vec2* vec2Copy(vec2 *pOut, const vec2 *pV) {
	memcpy(pOut, pV, sizeof(vec2));
	return pOut;
}

vec2* vec2_Orthogonal(vec2 *pOut, const vec2 *pV) {
	pOut->v[0] = pV->v[1];
	pOut->v[1] = - pV->v[0];
	return pOut;
}

vec2* vec2Add(vec2 *pOut, const vec2 *pV1, const vec2 *pV2) {
	pOut->v[0] = pV1->v[0] + pV2->v[0];
	pOut->v[1] = pV1->v[1] + pV2->v[1];
	return pOut; 
}

vec2* vec2Sub(vec2 *pOut, const vec2 *pV1, const vec2 *pV2) { 
	pOut->v[0] = pV1->v[0] - pV2->v[0];
	pOut->v[1] = pV1->v[1] - pV2->v[1];
	return pOut; 
}

vec2* vec2Scale(vec2 *pOut, const vec2 *pV, float fScale) { 
	pOut->v[0] = pV->v[0] * fScale;
	pOut->v[1] = pV->v[1] * fScale;
	return pOut; 
}

vec2* vec2Normalize(vec2 *pOut, const vec2 *pV) {
	return vec2Scale(pOut, pV, 1 / vec2Length(pV));
}

float vec2Dot(const vec2 *pV1, const vec2 *pV2) {
	return pV1->v[0] * pV2->v[0] + pV1->v[1] * pV2->v[1];
}

float vec2Length(const vec2 *pV) {
	float fLength2 = (pV->v[0] * pV->v[0] + pV->v[1] * pV->v[1]);
	float l = sqrt(fLength2);
	printf("%.2f, %.2f, %.2f\n", fLength2, (float) sqrt(fLength2), l);
	return l;
}

vec2* segment2_IntersectParallel(vec2 *pOut, float *t1, float *t2,
												 const segment2 *s1, const segment2 *s2) {
	return NULL;
}

vec2* segment2_IntersectNonParallel(vec2 *pOut, float *t1, float *t2,
												 const segment2 *s1, const segment2 *s2) {
	vec3 v1, v2;
	vec3 tmp1, tmp2;
	vec3 vIntersection;
	
	tmp1.v[0] = s1->vStart.v[0];
	tmp1.v[1] = s1->vStart.v[1];
	tmp1.v[2] = 1;
	tmp2.v[0] = s1->vStart.v[0] + s1->vDirection.v[0];
	tmp2.v[1] = s1->vStart.v[1] + s1->vDirection.v[1];
	tmp2.v[2] = 1;
	vec3Cross(&v1, &tmp1, &tmp2);

	tmp1.v[0] = s2->vStart.v[0];
	tmp1.v[1] = s2->vStart.v[1];
	tmp1.v[2] = 1;
	tmp2.v[0] = s2->vStart.v[0] + s2->vDirection.v[0];
	tmp2.v[1] = s2->vStart.v[1] + s2->vDirection.v[1];
	tmp2.v[2] = 1;
	vec3Cross(&v2, &tmp1, &tmp2);
	vec3Cross(&vIntersection, &v1, &v2);
	pOut->v[0] = vIntersection.v[0] / vIntersection.v[2];
	pOut->v[1] = vIntersection.v[1] / vIntersection.v[2];
	
	// TODO compute t1, t2
	if(fabs(s1->vDirection.v[0]) > fabs(s1->vDirection.v[1]))
		*t1 = (pOut->v[0] - s1->vStart.v[0]) / s1->vDirection.v[0];
	else
		*t1 = (pOut->v[1] - s1->vStart.v[1]) / s1->vDirection.v[1];
	if(fabs(s2->vDirection.v[0]) > fabs(s2->vDirection.v[1]))
		*t2 = (pOut->v[0] - s2->vStart.v[0]) / s2->vDirection.v[0];
	else
		*t2 = (pOut->v[1] - s2->vStart.v[1]) / s2->vDirection.v[1];
	
	return pOut;
}

vec2* segment2_Intersect(vec2 *pOut, float *t1, float *t2,
												 const segment2 *s1, const segment2 *s2) {
	// check if s1, s2 are parallel
	vec2 tmp;
	if( vec2Dot(&s1->vDirection, 
							vec2_Orthogonal(&tmp, & s2->vDirection) ) == 0)
		return segment2_IntersectParallel(pOut, t1, t2, s1, s2);
	else
		return segment2_IntersectNonParallel(pOut, t1, t2, s1, s2);	
}

