#include "video/nebu_camera.h"
#include "video/nebu_gl.h"
#include "base/nebu_math.h"
#include "base/nebu_matrix.h"

#include <stdio.h>

static void doLookAt(const float *cam, const float *target, const float *up) {
  matrix m;
  vec3 x, y, z;

	vec3_Sub(&z, (vec3*) cam, (vec3*) target);
	vec3_Normalize(&z, &z);
	vec3_Cross(&x, (vec3*) up, &z);
	vec3_Normalize(&x, &x);
	vec3_Cross(&y, &z, &x);
	vec3_Normalize(&y, &y);

#define M(row,col)  m.m[col*4+row]
  M(0,0) = x.v[0];  M(0,1) = x.v[1];  M(0,2) = x.v[2];  M(0,3) = 0.0;
  M(1,0) = y.v[0];  M(1,1) = y.v[1];  M(1,2) = y.v[2];  M(1,3) = 0.0;
  M(2,0) = z.v[0];  M(2,1) = z.v[1];  M(2,2) = z.v[2];  M(2,3) = 0.0;
  M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
  glMultMatrixf( m.m );

  /* Translate Eye to Origin */
  glTranslatef( -cam[0], -cam[1], -cam[2]);
}

void nebu_Camera_GetRotationMatrix(nebu_Matrix4D *matrix, 
																	 const nebu_Camera *pCamera) {
  vec3 x, y, z;

	vec3_Sub(&z, &pCamera->vEye, &pCamera->vLookAt);
	vec3_Normalize(&z, &z);
	vec3_Cross(&x, &pCamera->vUp, &z);
	vec3_Normalize(&x, &x);
	vec3_Cross(&y, &z, &x);
	vec3_Normalize(&y, &y);

#define M(row,col)  matrix->m[col*4+row]
  M(0,0) = x.v[0];  M(0,1) = x.v[1];  M(0,2) = x.v[2];  M(0,3) = 0.0;
  M(1,0) = y.v[0];  M(1,1) = y.v[1];  M(1,2) = y.v[2];  M(1,3) = 0.0;
  M(2,0) = z.v[0];  M(2,1) = z.v[1];  M(2,2) = z.v[2];  M(2,3) = 0.0;
  M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
	return;
}

void nebu_Camera_LookAt(const nebu_Camera *pCamera) {
	doLookAt(pCamera->vEye.v, pCamera->vLookAt.v , pCamera->vUp.v);
}

void nebu_Camera_Zoom(nebu_Camera *pCamera, float d) {
	vec3 v;
	vec3_Sub(&v, &pCamera->vLookAt, &pCamera->vEye);
	vec3_Normalize(&v, &v);
	vec3_Scale(&v, &v, d);
	vec3_Add(&pCamera->vEye, &pCamera->vEye, &v);
}

void nebu_Camera_Roll(nebu_Camera *pCamera, float d) {
	vec3 v;
	matrix m;
	vec3_Sub(&v, &pCamera->vLookAt, &pCamera->vEye);
	vec3_Normalize(&v, &v);
	matrixRotationAxis(&m, d * 2 * M_PI / 360.0, &v);
	vec3_Transform(&pCamera->vUp, &pCamera->vUp, &m);
}

void nebu_Camera_RotateAroundTarget(nebu_Camera *pCamera,
																		float dx, float dy) {
	// adjust up vector, so that it is orthogonal to
	// view direction
	vec3 vDiff, vView, vRight, vUp;
	vec3 vdx, vdy;
	
	vec3_Sub(&vDiff, &pCamera->vLookAt, &pCamera->vEye);
	vec3_Normalize(&vView, &vDiff);
	vec3_Cross(&vRight, &pCamera->vUp, &vView);
	vec3_Normalize(&vRight, &vRight);
	vec3_Cross(&vUp, &vView, &vRight);
	vec3_Normalize(&vUp, &vUp);
	
	// horizontal movement (dx):
	if(dx == 0) {
		vec3_Zero(&vdx);
	} else {
		// rotate eye point around up vector through lookAt point
		vec3 v = vDiff;
		float fAngle = dx * 2 * M_PI / 360.0;
		matrix matRotation;
		matrixRotationAxis(&matRotation, fAngle, &vUp);
		vec3_Transform(&v, &vDiff, &matRotation);
		vec3_Sub(&vdx, &v, &vDiff);
	}

	// vertical movement (dy):
	if(dy == 0) {
		vec3_Zero(&vdy);
	} else {
		// rotate eye point around up vector through lookAt point
		vec3 v = vDiff;
		float fAngle = dy * 2 * M_PI / 360.0;
		matrix matRotation;
		matrixRotationAxis(&matRotation, fAngle, &vRight);
		vec3_Transform(&v, &vDiff, &matRotation);
		vec3_Sub(&vdy, &v, &vDiff);

		matrixTranspose(&matRotation, &matRotation);
		vec3_Transform(&pCamera->vUp, &pCamera->vUp, &matRotation);
	}

	// add relative movements to camera position
	
	/*
	vec3_Add(&pCamera->vEye, &pCamera->vEye, &vdx);
	vec3_Add(&pCamera->vEye, &pCamera->vEye, &vdy);
	*/
	{
		vec3 v;
		vec3_Add(&v, &vdx, &vdy);
		vec3_Add(&v, &v, &pCamera->vEye);
		vec3_Sub(&v, &v, &pCamera->vLookAt);
		vec3_Normalize(&v, &v);
		// printf("up dot view: %.4f\n", - vec3_Dot(&v, &pCamera->vUp));
		vec3_Scale(&v, &v, vec3_Length(&vDiff));
		vec3_Add(&pCamera->vEye, &v, &pCamera->vLookAt);

	}
}

void nebu_Camera_Print(const nebu_Camera *pCamera) {
	printf("LookAt: ");
	vec3_Print(&pCamera->vLookAt);
	printf("\n");
	printf("Eye: ");
	vec3_Print(&pCamera->vEye);
	printf("\n");
	printf("Up: ");
	vec3_Print(&pCamera->vUp);
	printf("\n");
}

