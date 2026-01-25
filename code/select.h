#ifndef SELECT_H
#define SELECT_H

#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmodel.h>
#include "game_state.h"
#include "scenes.h"

// Car data structure
typedef struct {
    const char* name;
    const char* model_path;
    T3DModel* model;
    const char* specs[6];
} CarData;

// Car + stage selection state
typedef struct {
    CarData cars[4];
    int current_car;
    int current_track;
    bool selecting_track;
    float rotation_y;
    T3DViewport viewport;
    T3DMat4FP* modelMat;
    rdpq_font_t* font;

    // Audio
    wav64_t music;

    // Lighting
    uint8_t colorAmbient[4];
    uint8_t colorDir[4];
    T3DVec3 lightDirVec;

    // Arrow UI state
    bool left_arrow_active;
    bool right_arrow_active;
} CarSelectScene;

void select_init(CarSelectScene* scene, rdpq_font_t* font, const GameState* state);
int select_update(CarSelectScene* scene, GameState* state);
void select_render(CarSelectScene* scene);
void select_cleanup(CarSelectScene* scene);

#endif // SELECT_H
