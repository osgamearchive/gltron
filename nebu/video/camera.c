#include "video/nebu_camera.h"
#include "video/nebu_gl.h"
#include "base/nebu_math.h"
#include "base/nebu_matrix.h"

static void doLookAt(float *cam, float *target, float *up) {
  float m[16];
  vec3 x, y, z;

	vec3_Sub(&z, (vec3*) cam, (vec3*) target);
	vec3_Normalize(&z, &z);
	vec3_Cross(&x, (vec3*) up, &z);
	vec3_Normalize(&x, &x);
	vec3_Cross(&y, &z, &x);
	vec3_Normalize(&y, &y);

#define M(row,col)  m[col*4+row]
  M(0,0) = x.v[0];  M(0,1) = x.v[1];  M(0,2) = x.v[2];  M(0,3) = 0.0;
  M(1,0) = y.v[0];  M(1,1) = y.v[1];  M(1,2) = y.v[2];  M(1,3) = 0.0;
  M(2,0) = z.v[0];  M(2,1) = z.v[1];  M(2,2) = z.v[2];  M(2,3) = 0.0;
  M(3,0) = 0.0;   M(3,1) = 0.0;   M(3,2) = 0.0;   M(3,3) = 1.0;
#undef M
  glMultMatrixf( m );

  /* Translate Eye to Origin */
  glTranslatef( -cam[0], -cam[1], -cam[2]);
}

void nebu_Camera_LookAt(nebu_Camera *pCamera) {
	doLookAt(pCamera->vEye.v, pCamera->vLookAt.v , pCamera->vUp.v);
}

void nebu_Camera_RotateAroundTarget(nebu_Camera *pCamera,
																		int dx, int dy) {
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
		vec3Transform(&v, &vDiff, &matRotation);
		vec3_Sub(&vdx, &v, &vDiff);
	}
	// vertical movement (dy):
	if(dy == 0) {
		vec3_Zero(&vdy);
	} else {
		// rotate eye point around up vector through lookAt point
		vec3 v = vDiff;
		float fAngle = dx * 2 * M_PI / 360.0;
		matrix matRotation;
		matrixRotationAxis(&matRotation, fAngle, &vRight);
		vec3Transform(&v, &vDiff, &matRotation);
		vec3_Sub(&vdx, &v, &vDiff);
	}

	// add relative movements to camera position
	vec3_Add(&pCamera->vEye, &pCamera->vEye, &vdx);
	vec3_Add(&pCamera->vEye, &pCamera->vEye, &vdy);
}
