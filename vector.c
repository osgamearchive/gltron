#include "vector.h"
#include "math.h"
#include <stdio.h>

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

void vec4Print(vec4 *v) {
  int i;
  printf("[ ");
  for(i = 0; i < 4; i++) {
    printf("%.3f ", v->v[i]);
  }
  printf(" ]\n");
}

void vec3Print(vec3 *v) {
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

vec3* vec3Scale(vec3 *pV, float f) {
	pV->v[0] *= f;
	pV->v[1] *= f;
	pV->v[2] *= f;
	return pV;
}

unsigned int uintFromVec3(vec3 *pV) {
	return
		( ( (unsigned int)(pV->v[0] * 127.0f + 128.0f) ) << 16 ) +
		( ( (unsigned int)(pV->v[1] * 127.0f + 128.0f) ) << 8 ) +
		( ( (unsigned int)(pV->v[2] * 127.0f + 128.0f) ) << 0 );
}
		
