#ifndef CARMOVEMENT_H
#define CARMOVEMENT_H

#include <libdragon.h>
#include <t3d/t3d.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

typedef struct {
    T3DVec3 position;
    float rotation_y;
    float velocity_x;
    float velocity_z;
    float camera_rotation;

    float max_speed;
    float acceleration;
    float brake_power;
    float turn_speed;
    float friction;
    float grip;
    float drift_grip;
} CarMovement;

void carmovement_init(CarMovement* cm, T3DVec3 start_pos, float max_speed);
bool carmovement_update(CarMovement* cm, joypad_inputs_t inputs, joypad_buttons_t btn_held);

#endif // CARMOVEMENT_H
