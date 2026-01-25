#include "carmovement.h"
#include <math.h>

void carmovement_init(CarMovement* cm, T3DVec3 start_pos, float max_speed) {
    cm->position = start_pos;
    cm->rotation_y = M_PI;
    cm->velocity_x = 0.0f;
    cm->velocity_z = 0.0f;
    cm->camera_rotation = 0.0f;

    cm->max_speed = max_speed;
    cm->acceleration = 1.5f;
    cm->brake_power = 3.0f;
    cm->turn_speed = 0.08f;
    cm->friction = 0.95f;
    cm->grip = 0.90f;
    cm->drift_grip = 0.15f;
}

bool carmovement_update(CarMovement* cm, joypad_inputs_t inputs, joypad_buttons_t btn_held) {
    const float camera_rotation_speed = 0.1f;
    if (btn_held.c_left) {
        cm->camera_rotation -= camera_rotation_speed;
    }
    if (btn_held.c_right) {
        cm->camera_rotation += camera_rotation_speed;
    }

    float stick_x = inputs.stick_x / 128.0f;

    const float deadzone = 0.15f;
    if (fabs(stick_x) < deadzone) stick_x = 0.0f;

    bool is_accelerating = btn_held.a;
    bool is_braking = btn_held.b;

    // Get current speed
    float current_speed = sqrtf(cm->velocity_x * cm->velocity_x + cm->velocity_z * cm->velocity_z);
    
    // Determine if we're drifting based on turn intensity and speed
    float turn_intensity = fabs(stick_x);
    float speed_factor = current_speed / cm->max_speed;
    bool is_drifting = (turn_intensity > 0.3f && speed_factor > 0.3f);
    
    // Apply turning with dramatic oversteer when drifting
    if (current_speed > 0.5f) {
        float base_turn = stick_x * cm->turn_speed * speed_factor;
        
        // Add aggressive oversteer when drifting - scales exponentially with turn intensity
        float oversteer = 0.0f;
        if (is_drifting) {
            // Square turn intensity for more dramatic effect on sharp turns
            float drift_multiplier = turn_intensity * turn_intensity * 0.15f;
            oversteer = stick_x * drift_multiplier * speed_factor;
        }
        
        cm->rotation_y += base_turn + oversteer;
    }
    
    // Calculate forward direction based on car's facing angle
    float forward_x = fm_sinf(-cm->rotation_y);
    float forward_z = fm_cosf(-cm->rotation_y);
    
    // Apply acceleration/braking in forward direction
    if (is_accelerating) {
        cm->velocity_x += forward_x * cm->acceleration;
        cm->velocity_z += forward_z * cm->acceleration;
    } else if (is_braking) {
        cm->velocity_x -= forward_x * cm->brake_power;
        cm->velocity_z -= forward_z * cm->brake_power;
    }
    
    // Calculate right direction (perpendicular to forward)
    float right_x = fm_cosf(cm->rotation_y);
    float right_z = fm_sinf(cm->rotation_y);
    
    // Calculate lateral velocity (how much we're sliding sideways)
    float lateral_velocity = cm->velocity_x * right_x + cm->velocity_z * right_z;
    
    // Apply grip based on drifting state
    float current_grip = is_drifting ? cm->drift_grip : cm->grip;
    
    // Apply grip - remove lateral velocity to straighten out
    cm->velocity_x -= lateral_velocity * right_x * current_grip;
    cm->velocity_z -= lateral_velocity * right_z * current_grip;
    
    // Apply friction
    cm->velocity_x *= cm->friction;
    cm->velocity_z *= cm->friction;
    
    // Cap maximum speed
    current_speed = sqrtf(cm->velocity_x * cm->velocity_x + cm->velocity_z * cm->velocity_z);
    if (current_speed > cm->max_speed) {
        float scale = cm->max_speed / current_speed;
        cm->velocity_x *= scale;
        cm->velocity_z *= scale;
        current_speed = cm->max_speed;
    }
    
    // Stop completely if moving very slowly
    if (current_speed < 0.1f) {
        cm->velocity_x = 0.0f;
        cm->velocity_z = 0.0f;
    }

    bool is_moving = (current_speed > 0.1f);
    if (is_moving) {
        float move_x = cm->velocity_x;
        float move_z = cm->velocity_z;

        cm->position.v[0] += move_x;
        cm->position.v[2] += move_z;
    }

    return is_moving;
}
