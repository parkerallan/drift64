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
    CarData cars[4];
    int current_car;
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
    
    showcase.cars[3] = (CarData){
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
    
    // Load car models
    for (int i = 0; i < 4; i++) {
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
    wav64_open(&showcase.music, "rom:/KickingInDoors.wav64");
    wav64_play(&showcase.music, 0); // Channel 0
    
    // Set up brighter overhead lighting
    showcase.colorAmbient[0] = 120;  // R - increased ambient
    showcase.colorAmbient[1] = 120;  // G  
    showcase.colorAmbient[2] = 120;  // B - brighter neutral ambient
    showcase.colorAmbient[3] = 0xFF; // A
    
    showcase.colorDir[0] = 255;      // R - maximum directional light
    showcase.colorDir[1] = 255;      // G
    showcase.colorDir[2] = 255;      // B - bright white overhead light
    showcase.colorDir[3] = 0xFF;     // A
    
    // Overhead light direction (coming from above)
    showcase.lightDirVec = (T3DVec3){{0.0f, -1.0f, -0.2f}};
    t3d_vec3_norm(&showcase.lightDirVec);
    
    // Initialize arrow states
    showcase.left_arrow_active = false;
    showcase.right_arrow_active = false;
}

void draw_left_arrow(float x, float y, float size, bool is_active) {
    // Main arrow color
    color_t arrow_color = is_active ? 
        RGBA32(150, 255, 150, 255) :  // Bright glow green
        RGBA32(0, 200, 0, 255);        // Normal green
    
    // Draw halo effect when active (larger, semi-transparent triangle behind)
    if (is_active) {
        color_t halo_color = RGBA32(100, 200, 100, 128); // Semi-transparent green halo
        float halo_size = size * 1.4f;
        float halo_offset = (halo_size - size) / 2.0f; // Center the halo
        
        rdpq_set_prim_color(halo_color);
        
        // Halo triangle arrowhead pointing left (centered on main arrow)
        float halo_v1[] = { x - halo_offset, y };                           // Left point (tip)
        float halo_v2[] = { x + size + halo_offset, y - halo_size * 0.6f }; // Top right
        float halo_v3[] = { x + size + halo_offset, y + halo_size * 0.6f }; // Bottom right
        rdpq_triangle(&TRIFMT_FILL, halo_v1, halo_v2, halo_v3);
    }
    
    // Draw main arrow triangle
    rdpq_set_prim_color(arrow_color);
    
    // Triangle arrowhead pointing left
    float v1[] = { x, y };                       // Left point (tip)
    float v2[] = { x + size, y - size * 0.6f };  // Top right
    float v3[] = { x + size, y + size * 0.6f };  // Bottom right
    rdpq_triangle(&TRIFMT_FILL, v1, v2, v3);
}

void draw_right_arrow(float x, float y, float size, bool is_active) {
    // Main arrow color
    color_t arrow_color = is_active ? 
        RGBA32(150, 255, 150, 255) :  // Bright glow green
        RGBA32(0, 200, 0, 255);        // Normal green
    
    // Draw halo effect when active (larger, semi-transparent triangle behind)
    if (is_active) {
        color_t halo_color = RGBA32(100, 200, 100, 128); // Semi-transparent green halo
        float halo_size = size * 1.4f;
        float halo_offset = (halo_size - size) / 2.0f; // Center the halo
        
        rdpq_set_prim_color(halo_color);
        
        // Halo triangle arrowhead pointing right (centered on main arrow)
        float halo_v1[] = { x + size + halo_offset, y };                    // Right point (tip)
        float halo_v2[] = { x - halo_offset, y - halo_size * 0.6f };        // Top left
        float halo_v3[] = { x - halo_offset, y + halo_size * 0.6f };        // Bottom left
        rdpq_triangle(&TRIFMT_FILL, halo_v1, halo_v2, halo_v3);
    }
    
    // Draw main arrow triangle
    rdpq_set_prim_color(arrow_color);
    
    // Triangle arrowhead pointing right
    float v1[] = { x + size, y };                // Right point (tip)
    float v2[] = { x, y - size * 0.6f };         // Top left
    float v3[] = { x, y + size * 0.6f };         // Bottom left
    rdpq_triangle(&TRIFMT_FILL, v1, v2, v3);
}

void showcase_update() {
    joypad_poll();
    joypad_inputs_t inputs = joypad_get_inputs(JOYPAD_PORT_1);
    
    // Track arrow states for visual feedback
    showcase.left_arrow_active = (inputs.stick_x < -64);
    showcase.right_arrow_active = (inputs.stick_x > 64);
    
    // Handle car switching with analog stick
    static int stick_cooldown = 0;
    if (stick_cooldown > 0) stick_cooldown--;
    
    if (stick_cooldown == 0) {
        if (showcase.left_arrow_active) { // Left - previous car
            showcase.current_car = (showcase.current_car - 1 + 4) % 4;
            stick_cooldown = 15; // Prevent rapid switching
        } else if (showcase.right_arrow_active) { // Right - next car
            showcase.current_car = (showcase.current_car + 1) % 4;
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

    // Draw UI arrows
    rdpq_sync_pipe();
    rdpq_sync_tile();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    
    // Left arrow (closer to left edge, vertically centered)
    draw_left_arrow(10, 120, 18, showcase.left_arrow_active);
    
    // Right arrow (closer to right edge, vertically centered)
    draw_right_arrow(292, 120, 18, showcase.right_arrow_active);
    
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
    
    // Debug: Show arrow state
    // rdpq_set_prim_color(RGBA32(0xFF, 0xFF, 0x00, 0xFF));
    // rdpq_text_printf(NULL, 10, 16, 28, "L:%d R:%d", showcase.left_arrow_active, showcase.right_arrow_active);

    rdpq_detach_show();
}

void showcase_cleanup() {
    // Stop and close music
    // wav64_close(&showcase.music);
    
    for (int i = 0; i < 4; i++) {
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
    
    audio_init(48000, 16);  // 48kHz, 16 buffers
    mixer_init(16);        // 16 channels
    wav64_init_compression(3);  // Opus compression
    
    display_init(RESOLUTION_320x240, DEPTH_16_BPP, 2, GAMMA_NONE, FILTERS_RESAMPLE_ANTIALIAS);
    rdpq_init();
    joypad_init();
    
    // Initialize T3D
    t3d_init((T3DInitParams){});
    
    // Initialize showcase
    showcase_init();
    
    // Main loop
    while (1) {
        mixer_try_play();
        
        showcase_update();
        showcase_render();
    }
    
    showcase_cleanup();
    t3d_destroy();
    return 0;
}
