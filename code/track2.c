#include "track2.h"

static const char* k_car_models[] = {
    "rom:/f40.t3dm",
    "rom:/clk.t3dm",
    "rom:/m1.t3dm",
    "rom:/diablo.t3dm"
};

void track2_init(Track2Scene* scene, rdpq_font_t* font, const GameState* state) {
    scene->font = font;
    scene->viewport = t3d_viewport_create();
    scene->selected_car = (state && state->selected_car >= 0) ? state->selected_car : 0;
    scene->car_model = t3d_model_load(k_car_models[scene->selected_car % 4]);
    scene->track_model = t3d_model_load("rom:/track2.t3dm");
    scene->modelMat = malloc_uncached(sizeof(T3DMat4FP));
    t3d_mat4fp_identity(scene->modelMat);
    scene->trackMat = malloc_uncached(sizeof(T3DMat4FP));
    t3d_mat4fp_identity(scene->trackMat);
    carmovement_init(&scene->car_movement, (T3DVec3){{0.0f, 0.0f, 0.0f}}, 10.0f);
    scene->camera_settings = (CarCameraSettings){
        .distance = 220.0f,
        .height = 60.0f,
        .tilt_up = 20.0f,
        .smoothing = 0.85f,
        .current_rotation = M_PI
    };
    rdpq_text_register_font(10, scene->font);
}

int track2_update(Track2Scene* scene) {
    joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);
    joypad_buttons_t btn_held = joypad_get_buttons_held(JOYPAD_PORT_1);
    //joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    carmovement_update(&scene->car_movement, inputs, btn_held);


    t3d_viewport_set_projection(&scene->viewport, T3D_DEG_TO_RAD(60.0f), 5.0f, 1000.0f);
    carcamera_setup(&scene->viewport, &scene->car_movement.position, -scene->car_movement.rotation_y, &scene->camera_settings);

    float scale[3] = {1.0f, 1.0f, 1.0f};
    float rotation[3] = {0.0f, scene->car_movement.rotation_y, 0.0f};
    float position[3] = {scene->car_movement.position.v[0], scene->car_movement.position.v[1], scene->car_movement.position.v[2]};
    t3d_mat4fp_from_srt_euler(scene->modelMat, scale, rotation, position);

    return -1;
}

void track2_render(Track2Scene* scene) {
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&scene->viewport);

    t3d_screen_clear_color(RGBA32(60, 30, 30, 0xFF));
    t3d_screen_clear_depth();

    if (scene->track_model) {
        t3d_matrix_push(scene->trackMat);
        t3d_model_draw(scene->track_model);
        t3d_matrix_pop(1);
    }

    if (scene->car_model) {
        t3d_matrix_push(scene->modelMat);
        t3d_model_draw(scene->car_model);
        t3d_matrix_pop(1);
    }

    rdpq_sync_pipe();
    rdpq_text_printf(NULL, 10, 16, 16, "TRACK 2");
    rdpq_text_printf(NULL, 10, 16, 36, "Press B to return");

    rdpq_detach_show();
}

void track2_cleanup(Track2Scene* scene) {
    if (scene->car_model) {
        t3d_model_free(scene->car_model);
    }
    if (scene->track_model) {
        t3d_model_free(scene->track_model);
    }
    if (scene->modelMat) {
        free_uncached(scene->modelMat);
    }
    if (scene->trackMat) {
        free_uncached(scene->trackMat);
    }
    rdpq_text_unregister_font(10);
}
