#pragma once

#include <Mesen/api.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Should keep consistent with ControllerType (Core/Shared/SettingTypes.h)
enum MesenControllerType {
    MESEN_CONTROLLER_TYPE_NES_CONTROLLER = 6,
};

// Should keep consistent with ConsoleNotificationType (Core/Shared/SettingTypes.h)
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
    MESEN_NOTIFICATION_TYPE_EVENT_VIEWER_REFRESH,
    MESEN_NOTIFICATION_TYPE_MISSING_FIRMWARE,
    MESEN_NOTIFICATION_TYPE_SUFAMI_TURBO_FILE_PROMPT,
    MESEN_NOTIFICATION_TYPE_BEFORE_GAME_UNLOAD,
    MESEN_NOTIFICATION_TYPE_BEFORE_GAME_LOAD,
    MESEN_NOTIFICATION_TYPE_GAME_LOAD_FAILED,
    MESEN_NOTIFICATION_TYPE_CHEATS_CHANGED,
    MESEN_NOTIFICATION_TYPE_REQUEST_CONFIG_CHANGE,
    MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER,
    MESEN_NOTIFICATION_TYPE_START_AUDIO_DEVICE,
    MESEN_NOTIFICATION_TYPE_STOP_AUDIO_DEVICE,
};

// Should keep consistent with same as VideoFilterType (Core/Shared/SettingTypes.h)
enum MesenVideoFilterType {
    MESEN_VIDEO_FILTER_TYPE_NONE,
    MESEN_VIDEO_FILTER_TYPE_NTSC_BLARGG,
    MESEN_VIDEO_FILTER_TYPE_NTSC_BISQWIT,
    MESEN_VIDEO_FILTER_TYPE_LCD_GRID,
    MESEN_VIDEO_FILTER_TYPE_X_BRZ_2X,
    MESEN_VIDEO_FILTER_TYPE_X_BRZ_3X,
    MESEN_VIDEO_FILTER_TYPE_X_BRZ_4X,
    MESEN_VIDEO_FILTER_TYPE_X_BRZ_5X,
    MESEN_VIDEO_FILTER_TYPE_X_BRZ_6X,
    MESEN_VIDEO_FILTER_TYPE_HQ_2X,
    MESEN_VIDEO_FILTER_TYPE_HQ_3X,
    MESEN_VIDEO_FILTER_TYPE_HQ_4X,
    MESEN_VIDEO_FILTER_TYPE_SCALE_2X,
    MESEN_VIDEO_FILTER_TYPE_SCALE_3X,
    MESEN_VIDEO_FILTER_TYPE_SCALE_4X,
};

enum MesenHudDisplaySize {
    MESEN_HUD_DISPLAY_SIZE_FIXED,
    MESEN_HUD_DISPLAY_SIZE_SCALED,
};

enum MesenShortcutType {
    MESEN_SHORTCUT_TYPE_EXEC_RESET = 35,
};

typedef struct {
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

typedef struct {
    MesenKeyMapping key_mapping;
    MesenControllerType type;
} MesenControllerConfig;

typedef uint32_t MesenPalette[512];

typedef struct {
    const MesenPalette* user_palette;
    MesenControllerConfig port_1;
    MesenControllerConfig port_2;
} MesenNesConfig;

typedef void (*MesenNotificationCallback)(MesenNotificationType event_type, void* param);

typedef struct {
    uint32_t* buffer;
    uint32_t width;
    uint32_t height;
    bool is_dirty;
} MesenSoftwareRendererSurface;

typedef struct {
    MesenSoftwareRendererSurface frame;
    MesenSoftwareRendererSurface emulator_HUD;
    MesenSoftwareRendererSurface script_HUD;
} MesenSoftwareRendererFrame;

typedef struct {
    MesenVideoFilterType video_filter;
    double brightness;
    double contrast;
    double hue;
    double saturation;
} MesenVideoConfig;

typedef struct {
    MesenHudDisplaySize HUD_size;

    bool show_fps;
    bool show_frame_counter;
    bool show_game_timer;
    bool show_debug_info;
    bool disable_on_screen_display;

} MesenPreferences;

typedef struct {
    MesenShortcutType action;
    uint32_t param;
    void* param_ptr;
} MesenExecuteShortcutParams;

typedef struct {
    uint32_t sample_rate;
    bool is_stereo;
} MesenAudioDeviceParam;

MESEN_API extern const MesenPalette mesen_default_palette;

// /init & release
MESEN_API void mesen_init();
MESEN_API void mesen_initialize_emu(const char* home_folder,
    bool no_audio, bool no_video, bool no_input);

MESEN_API void mesen_release();

// important: set NES config
MESEN_API void mesen_set_NES_config(const MesenNesConfig* NES_config);

MESEN_API bool mesen_load_ROM(const char* file, const char* patch_file);
MESEN_API void mesen_load_recent_game(const char* file, bool reset_game);

MESEN_API bool mesen_is_running();
MESEN_API bool mesen_is_paused();
MESEN_API void mesen_stop();
MESEN_API void mesen_pause();
MESEN_API void mesen_resume();

MESEN_API uint32_t mesen_get_version();
MESEN_API const char* mesen_get_build_date();

MESEN_API void mesen_take_screenshot(const char* save_to_path);

MESEN_API double mesen_get_aspect_ratio();

MESEN_API void mesen_set_output_to_stdout(bool enable);
MESEN_API void mesen_display_message(const char* title, const char* msg, const char* param1, const char* param2);

// register / unregister notification callback
MESEN_API void* mesen_register_notification_callback(MesenNotificationCallback);
MESEN_API void mesen_unregister_notification_callback(void* handle);
MESEN_API const char* mesen_get_notification_type_name(int noti_type);

// input: tell emulator key state
MESEN_API void mesen_set_key_state(uint16_t scan_code, bool state);
MESEN_API void mesen_reset_key_state();

// config API
MESEN_API void mesen_set_video_config(const MesenVideoConfig* video_config);
MESEN_API void mesen_set_preferences(const MesenPreferences* preferences);

MESEN_API void mesen_execute_shortcut(const MesenExecuteShortcutParams* exec_params);

MESEN_API size_t mesen_get_log(char* out_buffer, size_t max_length);

MESEN_API const char* mesen_get_recent_games_folder();

MESEN_API void mesen_fill_audio_buffer(void* out_buffer, size_t len);

#ifdef __cplusplus
}
#endif
