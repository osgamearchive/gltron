#ifndef CAMERA_H
#define CAMERA_H

typedef struct CameraType {
    int interpolated_cam;
    int interpolated_target;
    int coupled;
    int freedom[3];
    int type;
} CameraType;

typedef enum eCamAxis { CAM_R = 0, CAM_CHI, CAM_PHI, CAM_PHI_OFFSET, eCamAxisCount } eCamAxis;

typedef struct Camera {
    float cam[3]; // the camera position
    float target[3]; // location the camera points to
    // up: the camera's up vector is always 0, 0, 1
    float movement[eCamAxisCount]; /* indices CAM_R, CAM_CHI, CAM_PHI, CAM_PHI_OFFSET */
    float pUser[16]; // static user data
    void *pUserData; // dynamic user data
    int bIsGlancing;
    // bIsGlancing: values are 0 (straight ahead), 1 (left), -1 (right)
    // players may glance left/right by pressing the appropriate keys
    // to check if it's safe to turn
    CameraType type;
} Camera;

void camera_ResetAll(void);
void nextCameraType(void);
void doCameraMovement(void);
void toggleCameraLock(void);

float getAngle(int dir);
float getInterpolatedAngle(float t, float last, float now);

#endif
