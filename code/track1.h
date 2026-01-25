#ifndef TRACK1_H
#define TRACK1_H

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "game_state.h"
#include "carmovement.h"
#include "carcamera.h"
#include "scenes.h"

typedef struct {
    rdpq_font_t* font;
    T3DViewport viewport;
    int selected_car;
    T3DModel* car_model;
    T3DMat4FP* modelMat;
    T3DModel* track_model;
    T3DMat4FP* trackMat;
    CarMovement car_movement;
    CarCameraSettings camera_settings;
} Track1Scene;

void track1_init(Track1Scene* scene, rdpq_font_t* font, const GameState* state);
int track1_update(Track1Scene* scene);
void track1_render(Track1Scene* scene);
void track1_cleanup(Track1Scene* scene);

#endif // TRACK1_H
