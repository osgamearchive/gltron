#ifndef NEBU_CAMERA_H
#define NEBU_CAMERA_H

#include "base/nebu_vector.h"

typedef struct {
	vec3 vEye, vLookAt, vUp;
} nebu_Camera;

void nebu_Camera_LookAt(nebu_Camera *pCamera);
void nebu_Camera_RotateAroundTarget(nebu_Camera *pCamera,
																		int dx, int dy);
#endif
