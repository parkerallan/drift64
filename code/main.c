#include <libdragon.h>
#include <t3d/t3d.h>
#include <t3d/t3dmath.h>
#include <t3d/t3dmodel.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Car data structure
typedef struct {
    const char* name;
    const char* model_path;
    T3DModel* model;
    const char* specs[6]; // Array of spec strings
} CarData;

// Car showcase state
typedef struct {
    CarData cars[3];
    int current_car;
    float rotation_y;
    T3DViewport viewport;
    T3DMat4FP* modelMat;
    rdpq_font_t* font;
    
    // Audio
    // wav64_t music;
    
    // Lighting
    uint8_t colorAmbient[4];
    uint8_t colorDir[4];
    T3DVec3 lightDirVec;
} CarShowcase;

CarShowcase showcase;

void showcase_init() {
    // Initialize car data
    showcase.cars[0] = (CarData){
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
    
    showcase.cars[1] = (CarData){
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
    
    showcase.cars[2] = (CarData){
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
    
    // Load car models
    for (int i = 0; i < 3; i++) {
        showcase.cars[i].model = t3d_model_load(showcase.cars[i].model_path);
        if (!showcase.cars[i].model) {
            debugf("ERROR: Failed to load model %s\n", showcase.cars[i].model_path);
        } else {
            debugf("Successfully loaded model %s\n", showcase.cars[i].model_path);
        }
    }
    
    // Initialize state
    showcase.current_car = 0; // Start with F40
    showcase.rotation_y = 0.0f;
    
    // Set up camera
    showcase.viewport = t3d_viewport_create();
    
    // Allocate model matrix
    showcase.modelMat = malloc_uncached(sizeof(T3DMat4FP));
    t3d_mat4fp_identity(showcase.modelMat);
    
    // Load debug font
    showcase.font = rdpq_font_load_builtin(FONT_BUILTIN_DEBUG_MONO);
    rdpq_text_register_font(10, showcase.font);
    
    // Load and start music
    // wav64_open(&showcase.music, "rom:/KickingInDoors.wav64");
    // wav64_play(&showcase.music, 0); // Channel 0
    
    // Set up neutral overhead lighting
    showcase.colorAmbient[0] = 80;   // R
    showcase.colorAmbient[1] = 80;   // G  
    showcase.colorAmbient[2] = 80;   // B - neutral ambient
    showcase.colorAmbient[3] = 0xFF; // A
    
    showcase.colorDir[0] = 220;      // R
    showcase.colorDir[1] = 220;      // G
    showcase.colorDir[2] = 220;      // B - neutral white overhead light
    showcase.colorDir[3] = 0xFF;     // A
    
    // Overhead light direction (coming from above)
    showcase.lightDirVec = (T3DVec3){{0.0f, -1.0f, -0.2f}};
    t3d_vec3_norm(&showcase.lightDirVec);
}

void showcase_update() {
    joypad_poll();
    joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);
    
    // Handle car switching with analog stick
    static int stick_cooldown = 0;
    if (stick_cooldown > 0) stick_cooldown--;
    
    if (stick_cooldown == 0) {
        if (inputs.stick_x < -64) { // Left - previous car
            showcase.current_car = (showcase.current_car - 1 + 3) % 3;
            stick_cooldown = 15; // Prevent rapid switching
        } else if (inputs.stick_x > 64) { // Right - next car
            showcase.current_car = (showcase.current_car + 1) % 3;
            stick_cooldown = 15;
        }
    }
    
    // Continuous rotation (slow 360)
    showcase.rotation_y += 0.008f;
    if (showcase.rotation_y >= 2 * M_PI) {
        showcase.rotation_y -= 2 * M_PI;
    }
    
    const T3DVec3 camPos = {{0, 65.0f, 200.0f}};
    const T3DVec3 camTarget = {{0, 0, 0}};         // Looking at car at origin
    
    t3d_viewport_set_projection(&showcase.viewport, T3D_DEG_TO_RAD(60.0f), 5.0f, 200.0f);
    t3d_viewport_look_at(&showcase.viewport, &camPos, &camTarget, &(T3DVec3){{0,1,0}});
    
    // Update model matrix for current car
    float scale[3] = {1.0f, 1.0f, 1.0f};
    float rotation[3] = {0.0f, showcase.rotation_y, 0.0f};
    float position[3] = {0.0f, 0.0f, 0.0f}; // At origin
    
    t3d_mat4fp_from_srt_euler(showcase.modelMat, scale, rotation, position);
}

void showcase_render() {
    rdpq_attach(display_get(), display_get_zbuf());
    t3d_frame_start();
    t3d_viewport_attach(&showcase.viewport);

    // Clear screen with dark gray background
    t3d_screen_clear_color(RGBA32(20, 20, 20, 0xFF));
    t3d_screen_clear_depth();
    
    // Set render flags
    t3d_state_set_drawflags(T3D_FLAG_SHADED | T3D_FLAG_TEXTURED | T3D_FLAG_DEPTH);
    
    // Set up lighting
    t3d_light_set_ambient(showcase.colorAmbient);
    t3d_light_set_directional(0, showcase.colorDir, &showcase.lightDirVec);
    t3d_light_set_count(1);

    // Draw current car
    if (showcase.cars[showcase.current_car].model) {
        t3d_matrix_push(showcase.modelMat);
        rdpq_set_prim_color(RGBA32(255, 255, 255, 255));
        t3d_model_draw(showcase.cars[showcase.current_car].model);
        t3d_matrix_pop(1);
    }

    // Draw UI text - car specs in bottom left
    rdpq_sync_pipe();
    
    float posX = 16;
    float posY = 180; // Bottom area of 240p screen
    
    // Car name
    rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0xFF, 0xFF));
    rdpq_text_printf(NULL, 10, posX, posY, "%s", showcase.cars[showcase.current_car].name);
    posY += 12;
    
    // Car specs
    rdpq_set_prim_color(RGBA32(0xAA, 0xAA, 0xAA, 0xFF));
    for (int i = 0; i < 6; i++) {
        if (posY < 235) { // Don't go off screen
            rdpq_text_printf(NULL, 10, posX, posY, "%s", showcase.cars[showcase.current_car].specs[i]);
            posY += 8;
        }
    }
    
    // Controls hint
    rdpq_set_prim_color(RGBA32(0x66, 0x66, 0xFF, 0xFF));
    rdpq_text_printf(NULL, 10, 16, 16, "Analog Stick L/R: Switch Cars");

    rdpq_detach_show();
}

void showcase_cleanup() {
    // Stop and close music
    // wav64_close(&showcase.music);
    
    for (int i = 0; i < 3; i++) {
        if (showcase.cars[i].model) {
            t3d_model_free(showcase.cars[i].model);
        }
    }
    
    if (showcase.modelMat) {
        free_uncached(showcase.modelMat);
    }
}

int main() {
    // Initialize libdragon
    debug_init_isviewer();
    debug_init_usblog();
    asset_init_compression(2);
    dfs_init(DFS_DEFAULT_LOCATION);
    
    // audio_init(48000, 2); 
    // mixer_init(20);
    
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
    rdpq_init();
    joypad_init();
    
    // Initialize T3D
    t3d_init((T3DInitParams){});
    
    // Initialize showcase
    showcase_init();
    
    // Main loop
    while (1) {
        // Handle audio mixing
        // if (audio_can_write()) {
        //     short *buf = audio_write_begin();
        //     mixer_poll(buf, audio_get_buffer_length());
        //     audio_write_end();
        // }
        // mixer_try_play();
        
        showcase_update();
        showcase_render();
    }
    
    showcase_cleanup();
    t3d_destroy();
    return 0;
}
