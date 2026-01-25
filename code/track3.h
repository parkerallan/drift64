#ifndef TRACK3_H
#define TRACK3_H

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
} Track3Scene;

void track3_init(Track3Scene* scene, rdpq_font_t* font, const GameState* state);
int track3_update(Track3Scene* scene);
void track3_render(Track3Scene* scene);
void track3_cleanup(Track3Scene* scene);

#endif // TRACK3_H
