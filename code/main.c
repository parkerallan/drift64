#include <libdragon.h>
#include <t3d/t3d.h>
#include "scenes.h"
#include "game_state.h"
#include "intro.h"
#include "select.h"
#include "track1.h"
#include "track2.h"
#include "track3.h"
#include "track4.h"
#include "track5.h"

static IntroScene intro_scene;
static CarSelectScene select_scene;
static Track1Scene track1_scene;
static Track2Scene track2_scene;
static Track3Scene track3_scene;
static Track4Scene track4_scene;
static Track5Scene track5_scene;

static rdpq_font_t* builtin_font;
static GameState game_state;

int main() {
    debug_init_isviewer();
    debug_init_usblog();
    asset_init_compression(2);
    dfs_init(DFS_DEFAULT_LOCATION);

    audio_init(48000, 16);
    mixer_init(16);
    wav64_init_compression(3);

    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
    rdpq_init();
    joypad_init();

    t3d_init((T3DInitParams){});

    builtin_font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);

    game_state.selected_car = 0;
    game_state.selected_track = 0;

    GameScene current_scene = SCENE_INTRO;
    intro_init(&intro_scene, builtin_font);

    while (1) {
        joypad_poll();
        mixer_try_play();

        int transition = -1;
        switch (current_scene) {
            case SCENE_INTRO:
                transition = intro_update(&intro_scene);
                break;
            case SCENE_SELECT:
                transition = select_update(&select_scene, &game_state);
                break;
            case SCENE_TRACK_1:
                transition = track1_update(&track1_scene);
                break;
            case SCENE_TRACK_2:
                transition = track2_update(&track2_scene);
                break;
            case SCENE_TRACK_3:
                transition = track3_update(&track3_scene);
                break;
            case SCENE_TRACK_4:
                transition = track4_update(&track4_scene);
                break;
            case SCENE_TRACK_5:
                transition = track5_update(&track5_scene);
                break;
            default:
                break;
        }

        if (transition >= 0 && transition != current_scene) {
            switch (current_scene) {
                case SCENE_INTRO:
                    intro_cleanup(&intro_scene);
                    break;
                case SCENE_SELECT:
                    select_cleanup(&select_scene);
                    break;
                case SCENE_TRACK_1:
                    track1_cleanup(&track1_scene);
                    break;
                case SCENE_TRACK_2:
                    track2_cleanup(&track2_scene);
                    break;
                case SCENE_TRACK_3:
                    track3_cleanup(&track3_scene);
                    break;
                case SCENE_TRACK_4:
                    track4_cleanup(&track4_scene);
                    break;
                case SCENE_TRACK_5:
                    track5_cleanup(&track5_scene);
                    break;
                default:
                    break;
            }

            switch (transition) {
                case SCENE_INTRO:
                    intro_init(&intro_scene, builtin_font);
                    break;
                case SCENE_SELECT:
                    select_init(&select_scene, builtin_font, &game_state);
                    break;
                case SCENE_TRACK_1:
                    track1_init(&track1_scene, builtin_font, &game_state);
                    break;
                case SCENE_TRACK_2:
                    track2_init(&track2_scene, builtin_font, &game_state);
                    break;
                case SCENE_TRACK_3:
                    track3_init(&track3_scene, builtin_font, &game_state);
                    break;
                case SCENE_TRACK_4:
                    track4_init(&track4_scene, builtin_font, &game_state);
                    break;
                case SCENE_TRACK_5:
                    track5_init(&track5_scene, builtin_font, &game_state);
                    break;
                default:
                    break;
            }

            current_scene = (GameScene)transition;
        }

        switch (current_scene) {
            case SCENE_INTRO:
                intro_render(&intro_scene);
                break;
            case SCENE_SELECT:
                select_render(&select_scene);
                break;
            case SCENE_TRACK_1:
                track1_render(&track1_scene);
                break;
            case SCENE_TRACK_2:
                track2_render(&track2_scene);
                break;
            case SCENE_TRACK_3:
                track3_render(&track3_scene);
                break;
            case SCENE_TRACK_4:
                track4_render(&track4_scene);
                break;
            case SCENE_TRACK_5:
                track5_render(&track5_scene);
                break;
            default:
                break;
        }
    }

    t3d_destroy();
    return 0;
}
