#ifndef CAMERA_H
#define CAMERA_H

typedef struct CameraType {
	int interpolated_cam;
	int interpolated_target;
	int coupled;
	int freedom[3];
	int type;
} CameraType;

typedef struct Camera {
	float cam[3]; // the camera position
	float target[3]; // location the camera points to
    // up: the camera's up vector is always 0, 0, 1
	float movement[4]; /* indices CAM_R, CAM_CHI, CAM_PHI, CAM_PHI_OFFSET */
    float pUser[16]; // static user data
    void *pUserData; // dynamic user data
	int bIsGlancing;
    // bIsGlancing: values are 0 (straight ahead), 1 (left), -1 (right)
    // players may glance left/right by pressing the appropriate keys
    // to check if it's safe to turn
	CameraType type;
} Camera;

typedef enum eCamType { 
	CAM_CIRCLE = 0,
	CAM_FOLLOW,
	CAM_COCKPIT,
	CAM_FREE,
    CAM_OFFSET,
    eCamTypeCount,
} eCamType;

#define CAM_TYPE_CIRCLING 0
#define CAM_TYPE_FOLLOW 1
#define CAM_TYPE_COCKPIT 2
#define CAM_TYPE_MOUSE 3
#define CAM_TYPE_OFFSET 4

#define CAM_COUNT 5

typedef enum ECamAxis { CAM_R = 0, CAM_CHI, CAM_PHI, CAM_PHI_OFFSET, eCamAxisCount } ECamAxis;
extern char* eCamAxisNames[];
extern char* eCamTypeNames[];

void camera_ResetAll(void);
void nextCameraType(void);
void doCameraMovement(void);

float getAngle(int dir);
float getInterpolatedAngle(float t, float last, float now);

#endif
