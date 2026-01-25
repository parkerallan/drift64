#ifndef TRACK4_H
#define TRACK4_H

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
} Track4Scene;

void track4_init(Track4Scene* scene, rdpq_font_t* font, const GameState* state);
int track4_update(Track4Scene* scene);
void track4_render(Track4Scene* scene);
void track4_cleanup(Track4Scene* scene);

#endif // TRACK4_H
