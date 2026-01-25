#ifndef TRACK2_H
#define TRACK2_H

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
} Track2Scene;

void track2_init(Track2Scene* scene, rdpq_font_t* font, const GameState* state);
int track2_update(Track2Scene* scene);
void track2_render(Track2Scene* scene);
void track2_cleanup(Track2Scene* scene);

#endif // TRACK2_H
