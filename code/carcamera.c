#include "carcamera.h"
#include <libdragon.h>

void carcamera_setup(
    T3DViewport* viewport,
    const T3DVec3* car_position,
    float car_rotation,
    CarCameraSettings* settings
) {
    float rotation_diff = car_rotation - settings->current_rotation;

    while (rotation_diff > M_PI) rotation_diff -= 2.0f * M_PI;
    while (rotation_diff < -M_PI) rotation_diff += 2.0f * M_PI;

    settings->current_rotation += rotation_diff * (1.0f - settings->smoothing);

    float camera_angle = settings->current_rotation + M_PI;
    float cos_rot = fm_cosf(camera_angle);
    float sin_rot = fm_sinf(camera_angle);

    float cam_x = car_position->v[0] + sin_rot * settings->distance;
    float cam_y = car_position->v[1] + settings->height;
    float cam_z = car_position->v[2] + cos_rot * settings->distance;

    float target_x = car_position->v[0];
    float target_y = car_position->v[1] + settings->tilt_up;
    float target_z = car_position->v[2];

    t3d_viewport_look_at(
        viewport,
        &(T3DVec3){{cam_x, cam_y, cam_z}},
        &(T3DVec3){{target_x, target_y, target_z}},
        &(T3DVec3){{0, 1, 0}}
    );
}
