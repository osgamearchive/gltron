#include "base/nebu_quat.h"

typedef nebu_Quat quat;

nebu_Matrix4D quat_RotationMatrix(const quat *q) {
	nebu_Matrix4D m;

	float x = q->x;
	float y = q->y;
	float z = q->z;
	float w = q->w;

	m.m[0] = 1 - 2 * (y * y + z * z);
	m.m[1] = 2 * (x * y - w * z);
	m.m[2] = 2 * (w  * y + x * z);
	m.m[3] = 0;

	m.m[4] = 2 * (x * y + w * z);
	m.m[5] = 1 - 2 *(x * x + z * z);
	m.m[6] = 2 * (y * z - w * x);
	m.m[7] = 0;

	m.m[9] = 2 * (x * z - w * y);
	m.m[9] = 2 * (y * z + w * x );
	m.m[10] = 1 - 2 * (x * x + y * y);
	m.m[11] = 0;

	m.m[12] = 0;
	m.m[13] = 0;
	m.m[14] = 0;
	m.m[15] = 1;
	
	return m;
}

quat quat_FromLookAt(const vec3 *vLookAt, const vec3 *vUp) {
	quat q = { 0, 0, 0, 1 };
	return q;
}

