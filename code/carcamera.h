#ifndef CARCAMERA_H
#define CARCAMERA_H

#include <t3d/t3d.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    float distance;
    float height;
    float tilt_up;
    float smoothing;
    float current_rotation;
} CarCameraSettings;

void carcamera_setup(
    T3DViewport* viewport,
    const T3DVec3* car_position,
    float car_rotation,
    CarCameraSettings* settings
);

#endif // CARCAMERA_H
