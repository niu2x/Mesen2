#pragma once

#include <Mesen/api.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

MESEN_API uint32_t mesen_get_version();
MESEN_API const char* mesen_get_build_date();

MESEN_API void mesen_init();

MESEN_API void mesen_initialize_emu(const char* home_folder,
    bool no_audio, bool no_video, bool no_input);

MESEN_API void mesen_release();

MESEN_API bool mesen_load_ROM(const char* file, const char* patch_file);

MESEN_API bool mesen_is_running();
MESEN_API bool mesen_is_paused();
MESEN_API void mesen_stop();
MESEN_API void mesen_pause();
MESEN_API void mesen_resume();

MESEN_API void mesen_dump_ROM_info();

MESEN_API void mesen_take_screenshot();
MESEN_API void mesen_add_known_game_folder(const char* folder);

MESEN_API double mesen_get_aspect_ratio();

enum MesenControllerType {
    MESEN_CONTROLLER_TYPE_NES_CONTROLLER = 5,
};

typedef struct MesenKeyMapping {
    uint16_t A;
    uint16_t B;
    uint16_t up;
    uint16_t down;
    uint16_t left;
    uint16_t right;
    uint16_t start;
    uint16_t select;
    uint16_t turbo_A;
    uint16_t turbo_B;
    // range: 0-3
    uint32_t turbo_speed;
} MesenKeyMapping;

typedef struct MesenControllerConfig
{
    MesenKeyMapping key_mapping;
    int type;
} MesenControllerConfig;

typedef struct MesenNesConfig{
    uint32_t user_palette[512];
    MesenControllerConfig port_1;
    MesenControllerConfig port_2;
} MesenNesConfig;

MESEN_API void mesen_set_NES_config(const MesenNesConfig* NES_config);
MESEN_API void mesen_set_output_to_stdout(bool enable);

typedef void (*NotificationCallback)(int event_type, void* param);

// Should keep consistent with ConsoleNotificationType
enum MesenNotificationType {
    MESEN_NOTIFICATION_TYPE_GAME_LOADED,
    MESEN_NOTIFICATION_TYPE_STATE_LOADED,
    MESEN_NOTIFICATION_TYPE_GAME_RESET,
    MESEN_NOTIFICATION_TYPE_GAME_PAUSED,
    MESEN_NOTIFICATION_TYPE_GAME_RESUMED,
    MESEN_NOTIFICATION_TYPE_CODE_BREAK,
    MESEN_NOTIFICATION_TYPE_DEBUGGER_RESUMED,
    MESEN_NOTIFICATION_TYPE_PPU_FRAME_DONE,
    MESEN_NOTIFICATION_TYPE_RESOLUTION_CHANGED,
    MESEN_NOTIFICATION_TYPE_CONFIG_CHANGED,
    MESEN_NOTIFICATION_TYPE_EXECUTE_SHORTCUT,
    MESEN_NOTIFICATION_TYPE_RELEASE_SHORTCUT,
    MESEN_NOTIFICATION_TYPE_EMULATION_STOPPED,
    MESEN_NOTIFICATION_TYPE_BEFORE_EMULATION_STOP,
    MESEN_NOTIFICATION_TYPE_VIEWER_REFRESH,
    MESEN_NOTIFICATION_TYPE_EVENT_VIEWERREFRESH,
    MESEN_NOTIFICATION_TYPE_MISSING_FIRMWARE,
    MESEN_NOTIFICATION_TYPE_SUFAMI_TURBO_FILE_PROMPT,
    MESEN_NOTIFICATION_TYPE_BEFORE_GAME_UNLOAD,
    MESEN_NOTIFICATION_TYPE_BEFORE_GAME_LOAD,
    MESEN_NOTIFICATION_TYPE_GAME_LOAD_FAILED,
    MESEN_NOTIFICATION_TYPE_CHEATS_CHANGED,
    MESEN_NOTIFICATION_TYPE_REQUEST_CONFIG_CHANGE,
    MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER,
};

typedef struct MesenSoftwareRendererSurface
{
    uint32_t* buffer;
    uint32_t width;
    uint32_t height = 0;
    bool is_dirty;
} MesenSoftwareRendererSurface;

typedef struct MesenSoftwareRendererFrame {
    MesenSoftwareRendererSurface frame;
    MesenSoftwareRendererSurface emulator_HUD;
    MesenSoftwareRendererSurface script_HUD;
} MesenSoftwareRendererFrame;

MESEN_API void* mesen_register_notification_callback(NotificationCallback);
MESEN_API void mesen_unregister_notification_callback(void* handle);
MESEN_API const char* mesen_get_notification_type_name(int noti_type);

MESEN_API void mesen_set_key_state(uint16_t scan_code, bool state);
MESEN_API void mesen_reset_key_state();

#ifdef __cplusplus
}
#endif
