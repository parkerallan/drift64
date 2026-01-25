#ifndef INTRO_H
#define INTRO_H

#include <libdragon.h>
#include <t3d/t3d.h>
#include "scenes.h"

typedef struct {
    rdpq_font_t* font;
    float blink_timer;
    T3DViewport viewport;
} IntroScene;

void intro_init(IntroScene* scene, rdpq_font_t* font);
int intro_update(IntroScene* scene);
void intro_render(IntroScene* scene);
void intro_cleanup(IntroScene* scene);

#endif // INTRO_H
