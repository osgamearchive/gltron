#ifndef NEBU_CAMERA_H
#define NEBU_CAMERA_H

#include "base/nebu_vector.h"
#include "base/nebu_matrix.h"

typedef struct {
	vec3 vEye, vLookAt, vUp;
} nebu_Camera;

nebu_Camera* nebu_Camera_Create(void);
void nebu_Camera_SetupEyeUpLookAt(nebu_Camera *pCamera, vec3 *pEye, vec3 *pUp, vec3 *pLookAt);
void nebu_Camera_LookAt(const nebu_Camera *pCamera);
void nebu_Camera_RotateAroundTarget(nebu_Camera *pCamera,
	float dx, float dy);
void nebu_Camera_Zoom(nebu_Camera *pCamera, float d);
void nebu_Camera_Roll(nebu_Camera *pCamera, float d);

void nebu_Camera_GetRotationMatrix(nebu_Matrix4D *matrix, const nebu_Camera *pCamera);

void nebu_Camera_Print(const nebu_Camera *pCamera);
#endif
