#ifndef NEBU_VECTOR_H
#define NEBU_VECTOR_H

typedef struct { float v[2]; } vec2;
typedef struct { float v[3]; } vec3;
typedef struct { float v[4]; } vec4;

typedef struct { vec2 vStart, vDirection; } segment2;

vec4* vec4Add(vec4 *pOut, const vec4 *pV1, const vec4 *pV2);
vec4* vec4Sub(vec4 *pOut, const vec4 *pV1, const vec4 *pV2);

vec3* vec3Add(vec3 *pOut, const vec3 *pV1, const vec3 *pV2);
vec3* vec3Sub(vec3 *pOut, const vec3 *pV1, const vec3 *pV2);
vec3* vec3Cross(vec3 *pOut, const vec3 *pV1, const vec3 *pV2);

float vec4Dot(const vec4 *pV1, const vec4 *pV2);

float vec3Dot(const vec3 *pV1, const vec3 *pV2);

float vec3Length(const vec3 *pV);
float vec3LengthSqr(const vec3 *pV);
vec3* vec3Normalize(vec3 *pOut, const vec3 *pV);

vec3* vec3fromVec4(vec3 *pOut, const vec4 *pV);
vec4* vec4fromVec3(vec4 *pOut, const vec3 *pV);

vec3* vec3Copy(vec3 *pOut, const vec3 *pV);
vec3* vec3Scale(vec3 *pOut, const vec3 *pV, float f);
void vec4Print(const vec4 *pV);
void vec3Print(const vec3 *pV);
vec3* vec3Zero(vec3 *pV);
vec3* vec3TriNormalDirection(vec3* pOut, const vec3* pV1, const vec3* pV2, const vec3 *pV3);

vec2* vec2Copy(vec2 *pOut, const vec2 *pV);

unsigned int uintFromVec3(vec3 *pV);
// vec4* vec4Transform(vec4* pOut, const vec4* pV, const matrix16 *pM);

vec2* segment2_Intersect(vec2 *pOut, float *t1, float *t2,
												 const segment2 *s1, const segment2 *s2);

vec2* vec2_Orthogonal(vec2 *pOut, const vec2 *pV);

vec2* vec2Add(vec2 *pOut, const vec2 *pV1, const vec2 *pV2);
vec2* vec2Sub(vec2 *pOut, const vec2 *pV1, const vec2 *pV2);
vec2* vec2Scale(vec2 *pOut, const vec2 *pV, float fScale);
vec2* vec2Normalize(vec2 *pOut, const vec2 *pV);
float vec2Dot(const vec2 *pV1, const vec2 *pV2);

float vec2Length(const vec2 *pV);
float segment2_Length(const segment2 *s);
#endif
