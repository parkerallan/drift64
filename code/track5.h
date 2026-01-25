#ifndef TRACK5_H
#define TRACK5_H

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
} Track5Scene;

void track5_init(Track5Scene* scene, rdpq_font_t* font, const GameState* state);
int track5_update(Track5Scene* scene);
void track5_render(Track5Scene* scene);
void track5_cleanup(Track5Scene* scene);

#endif // TRACK5_H
