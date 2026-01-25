#include "select.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static const char* k_track_names[5] = {
    "Track 1",
    "Track 2",
    "Track 3",
    "Track 4",
    "Track 5"
};

static GameScene track_scene_for_index(int track_index) {
    switch (track_index) {
        case 0: return SCENE_TRACK_1;
        case 1: return SCENE_TRACK_2;
        case 2: return SCENE_TRACK_3;
        case 3: return SCENE_TRACK_4;
        case 4: return SCENE_TRACK_5;
        default: return SCENE_TRACK_1;
    }
}

static void draw_left_arrow(float x, float y, float size, bool is_active) {
    color_t arrow_color = is_active ? RGBA32(150, 255, 150, 255) : RGBA32(0, 200, 0, 255);

    if (is_active) {
        color_t halo_color = RGBA32(100, 200, 100, 128);
        float halo_size = size * 1.4f;
        float halo_offset = (halo_size - size) / 2.0f;

        rdpq_set_prim_color(halo_color);
        float halo_v1[] = { x - halo_offset, y };
        float halo_v2[] = { x + size + halo_offset, y - halo_size * 0.6f };
        float halo_v3[] = { x + size + halo_offset, y + halo_size * 0.6f };
        rdpq_triangle(&TRIFMT_FILL, halo_v1, halo_v2, halo_v3);
    }

    rdpq_set_prim_color(arrow_color);
    float v1[] = { x, y };
    float v2[] = { x + size, y - size * 0.6f };
    float v3[] = { x + size, y + size * 0.6f };
    rdpq_triangle(&TRIFMT_FILL, v1, v2, v3);
}

static void draw_right_arrow(float x, float y, float size, bool is_active) {
    color_t arrow_color = is_active ? RGBA32(150, 255, 150, 255) : RGBA32(0, 200, 0, 255);

    if (is_active) {
        color_t halo_color = RGBA32(100, 200, 100, 128);
        float halo_size = size * 1.4f;
        float halo_offset = (halo_size - size) / 2.0f;

        rdpq_set_prim_color(halo_color);
        float halo_v1[] = { x + size + halo_offset, y };
        float halo_v2[] = { x - halo_offset, y - halo_size * 0.6f };
        float halo_v3[] = { x - halo_offset, y + halo_size * 0.6f };
        rdpq_triangle(&TRIFMT_FILL, halo_v1, halo_v2, halo_v3);
    }

    rdpq_set_prim_color(arrow_color);
    float v1[] = { x + size, y };
    float v2[] = { x, y - size * 0.6f };
    float v3[] = { x, y + size * 0.6f };
    rdpq_triangle(&TRIFMT_FILL, v1, v2, v3);
}

void select_init(CarSelectScene* scene, rdpq_font_t* font, const GameState* state) {
    scene->cars[0] = (CarData){
        .name = "Ferrari F40",
        .model_path = "rom:/f40.t3dm",
        .specs = {
            "Engine: 2.9L Twin-Turbo V8",
            "Power: 471 HP @ 7000 RPM",
            "Torque: 577 Nm @ 4000 RPM",
            "Weight: 1100 kg",
            "Top Speed: 324 km/h",
            "0-100: 4.1 seconds"
        }
    };

    scene->cars[1] = (CarData){
        .name = "Mercedes CLK GTR",
        .model_path = "rom:/clk.t3dm",
        .specs = {
            "Engine: 6.9L Naturally Aspirated V12",
            "Power: 612 HP @ 6800 RPM",
            "Torque: 731 Nm @ 5250 RPM",
            "Weight: 1440 kg",
            "Top Speed: 320 km/h",
            "0-100: 3.8 seconds"
        }
    };

    scene->cars[2] = (CarData){
        .name = "BMW M1",
        .model_path = "rom:/m1.t3dm",
        .specs = {
            "Engine: 3.5L Naturally Aspirated I6",
            "Power: 277 HP @ 6500 RPM",
            "Torque: 330 Nm @ 5000 RPM",
            "Weight: 1300 kg",
            "Top Speed: 262 km/h",
            "0-100: 5.6 seconds"
        }
    };

    scene->cars[3] = (CarData){
        .name = "Lamborghini Diablo VT",
        .model_path = "rom:/diablo.t3dm",
        .specs = {
            "Engine: 6.0L Naturally Aspirated V12",
            "Power: 550 HP @ 7000 RPM",
            "Torque: 580 Nm @ 5200 RPM",
            "Weight: 1625 kg",
            "Top Speed: 325 km/h",
            "0-100: 4.0 seconds"
        }
    };

    for (int i = 0; i < 4; i++) {
        scene->cars[i].model = t3d_model_load(scene->cars[i].model_path);
        if (!scene->cars[i].model) {
            debugf("ERROR: Failed to load model %s\n", scene->cars[i].model_path);
        } else {
            debugf("Successfully loaded model %s\n", scene->cars[i].model_path);
        }
    }

    scene->current_car = (state && state->selected_car >= 0) ? state->selected_car : 0;
    scene->current_track = (state && state->selected_track >= 0) ? state->selected_track : 0;
    scene->selecting_track = false;
    scene->rotation_y = 0.0f;

    scene->viewport = t3d_viewport_create();
    scene->modelMat = malloc_uncached(sizeof(T3DMat4FP));
    t3d_mat4fp_identity(scene->modelMat);

    scene->font = font;
    rdpq_text_register_font(10, scene->font);

    wav64_open(&scene->music, "rom:/KickingInDoors.wav64");
    wav64_play(&scene->music, 0);

    scene->colorAmbient[0] = 120;
    scene->colorAmbient[1] = 120;
    scene->colorAmbient[2] = 120;
    scene->colorAmbient[3] = 0xFF;

    scene->colorDir[0] = 255;
    scene->colorDir[1] = 255;
    scene->colorDir[2] = 255;
    scene->colorDir[3] = 0xFF;

    scene->lightDirVec = (T3DVec3){{0.0f, -1.0f, -0.2f}};
    t3d_vec3_norm(&scene->lightDirVec);

    scene->left_arrow_active = false;
    scene->right_arrow_active = false;
}

int select_update(CarSelectScene* scene, GameState* state) {
    joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    scene->left_arrow_active = (inputs.stick_x < -64);
    scene->right_arrow_active = (inputs.stick_x > 64);

    static int stick_cooldown = 0;
    if (stick_cooldown > 0) stick_cooldown--;

    if (stick_cooldown == 0) {
        if (scene->left_arrow_active) {
            if (scene->selecting_track) {
                scene->current_track = (scene->current_track + 4) % 5;
            } else {
                scene->current_car = (scene->current_car - 1 + 4) % 4;
            }
            stick_cooldown = 15;
        } else if (scene->right_arrow_active) {
            if (scene->selecting_track) {
                scene->current_track = (scene->current_track + 1) % 5;
            } else {
                scene->current_car = (scene->current_car + 1) % 4;
            }
            stick_cooldown = 15;
        }
    }

    if (btn.start) {
        if (scene->selecting_track) {
            if (state) {
                state->selected_car = scene->current_car;
                state->selected_track = scene->current_track;
            }
            return track_scene_for_index(scene->current_track);
        }
        scene->selecting_track = true;
    }

    if (btn.b) {
        if (scene->selecting_track) {
            scene->selecting_track = false;
        } else {
            return SCENE_INTRO;
        }
    }

    scene->rotation_y += 0.008f;
    if (scene->rotation_y >= 2 * M_PI) {
        scene->rotation_y -= 2 * M_PI;
    }

    const T3DVec3 camPos = {{0, 65.0f, 200.0f}};
    const T3DVec3 camTarget = {{0, 0, 0}};

    t3d_viewport_set_projection(&scene->viewport, T3D_DEG_TO_RAD(60.0f), 5.0f, 200.0f);
    t3d_viewport_look_at(&scene->viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});

    float scale[3] = {1.0f, 1.0f, 1.0f};
    float rotation[3] = {0.0f, scene->rotation_y, 0.0f};
    float position[3] = {0.0f, 0.0f, 0.0f};

    t3d_mat4fp_from_srt_euler(scene->modelMat, scale, rotation, position);

    return -1;
}

void select_render(CarSelectScene* scene) {
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&scene->viewport);

    t3d_screen_clear_color(RGBA32(20, 20, 20, 0xFF));
    t3d_screen_clear_depth();

    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_TEXTURED | T3D_FLAG_DEPTH);
    t3d_light_set_ambient(scene->colorAmbient);
    t3d_light_set_directional(0, scene->colorDir, &scene->lightDirVec);
    t3d_light_set_count(1);

    if (!scene->selecting_track && scene->cars[scene->current_car].model) {
        t3d_matrix_push(scene->modelMat);
        rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
        t3d_model_draw(scene->cars[scene->current_car].model);
        t3d_matrix_pop(1);
    }

    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);

    if (scene->selecting_track) {
        draw_left_arrow(10, 120, 18, scene->left_arrow_active);
        draw_right_arrow(292, 120, 18, scene->right_arrow_active);
        rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
        rdpq_text_printf(NULL, 10, 98, 108, "TRACK %d", scene->current_track + 1);
        rdpq_set_prim_color(RGBA32(0x99, 0x99, 0x99, 0xFF));
        rdpq_text_printf(NULL, 10, 98, 124, "(image placeholder)");
    } else {
        draw_left_arrow(10, 120, 18, scene->left_arrow_active);
        draw_right_arrow(292, 120, 18, scene->right_arrow_active);
    }

    rdpq_sync_pipe();

    if (!scene->selecting_track) {
        float posX = 16;
        float posY = 180;

        rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
        rdpq_text_printf(NULL, 10, posX, posY, "%s", scene->cars[scene->current_car].name);
        posY += 12;

        rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xAA, 0xFF));
        for (int i = 0; i < 6; i++) {
            if (posY < 235) {
                rdpq_text_printf(NULL, 10, posX, posY, "%s", scene->cars[scene->current_car].specs[i]);
                posY += 8;
            }
        }
    }

    rdpq_set_prim_color(RGBA32(0x66, 0x66, 0xFF, 0xFF));
    if (scene->selecting_track) {
        rdpq_text_printf(NULL, 10, 16, 16, "Track Select");
        rdpq_text_printf(NULL, 10, 16, 28, "Track: %s", k_track_names[scene->current_track]);
        rdpq_text_printf(NULL, 10, 16, 40, "Start: Begin  B: Car Select");
    } else {
        rdpq_text_printf(NULL, 10, 16, 16, "Car Select");
        rdpq_text_printf(NULL, 10, 16, 28, "Analog L/R: Switch Cars");
        rdpq_text_printf(NULL, 10, 16, 40, "Start: Track Select  B: Intro");
    }

    rdpq_detach_show();
}

void select_cleanup(CarSelectScene* scene) {
    for (int i = 0; i < 4; i++) {
        if (scene->cars[i].model) {
            t3d_model_free(scene->cars[i].model);
        }
    }

    if (scene->modelMat) {
        free_uncached(scene->modelMat);
    }

    wav64_close(&scene->music);
    rdpq_text_unregister_font(10);
}
