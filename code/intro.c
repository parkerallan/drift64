#include "intro.h"

void intro_init(IntroScene* scene, rdpq_font_t* font) {
    scene->font = font;
    scene->blink_timer = 0.0f;
    scene->viewport = t3d_viewport_create();
    rdpq_text_register_font(10, scene->font);
}

int intro_update(IntroScene* scene) {
    float current_time = (float)((double)get_ticks_us() / 1000000.0);
    scene->blink_timer = current_time;

    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);
    if (btn.start) {
        return SCENE_SELECT;
    }

    return -1;
}

void intro_render(IntroScene* scene) {
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&scene->viewport);

    t3d_screen_clear_color(RGBA32(10, 10, 20, 0xFF));
    t3d_screen_clear_depth();

    rdpq_sync_pipe();
    rdpq_text_printf(NULL, 10, 20, 40, "DRIFT 64");
    rdpq_text_printf(NULL, 10, 20, 60, "Car + Stage Selection");

    if (((int)(scene->blink_timer * 2.0f)) % 2 == 0) {
        rdpq_text_printf(NULL, 10, 20, 120, "Press Start");
    }

    rdpq_detach_show();
}

void intro_cleanup(IntroScene* scene) {
    rdpq_text_unregister_font(10);
}
