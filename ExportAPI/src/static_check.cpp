#include "Core/Shared/Interfaces/INotificationListener.h"
#include "Core/Shared/SettingTypes.h"
#include <Mesen/Mesen.h>
#include <cassert>

#define CHECK_ENUM(c_prefix, c_name, cpp_prefix, cpp_name)                                         \
    static_assert(c_prefix##c_name == (int)cpp_prefix ::cpp_name)

#define CHECK_CONTROLLER_TYPE(c_name, cpp_name)                                                    \
    CHECK_ENUM(MESEN_CONTROLLER_TYPE_, c_name, ControllerType, cpp_name)

#define CHECK_NOTIFICATION_TYPE(c_name, cpp_name)                                                  \
    CHECK_ENUM(MESEN_NOTIFICATION_TYPE_, c_name, ConsoleNotificationType, cpp_name)

#define CHECK_VIDEO_FILTER_TYPE(c_name, cpp_name)                                                  \
    CHECK_ENUM(MESEN_VIDEO_FILTER_TYPE_, c_name, VideoFilterType, cpp_name)

#define CHECK_HUD_DISPLAY_SIZE(c_name, cpp_name)                                                   \
    CHECK_ENUM(MESEN_HUD_DISPLAY_SIZE_, c_name, HudDisplaySize, cpp_name)

#define CHECK_SHORTCUT(c_name, cpp_name)                                                           \
    CHECK_ENUM(MESEN_SHORTCUT_TYPE_, c_name, EmulatorShortcut, cpp_name)

#define CHECK_STRUCT(A, B) static_assert(sizeof(A) == sizeof(B));

static void check() {
    CHECK_CONTROLLER_TYPE(NES_CONTROLLER, NesController);

    CHECK_NOTIFICATION_TYPE(GAME_LOADED, GameLoaded);
    CHECK_NOTIFICATION_TYPE(STATE_LOADED, StateLoaded);
    CHECK_NOTIFICATION_TYPE(GAME_RESET, GameReset);
    CHECK_NOTIFICATION_TYPE(GAME_PAUSED, GamePaused);
    CHECK_NOTIFICATION_TYPE(GAME_RESUMED, GameResumed);
    CHECK_NOTIFICATION_TYPE(CODE_BREAK, CodeBreak);
    CHECK_NOTIFICATION_TYPE(DEBUGGER_RESUMED, DebuggerResumed);
    CHECK_NOTIFICATION_TYPE(PPU_FRAME_DONE, PpuFrameDone);
    CHECK_NOTIFICATION_TYPE(RESOLUTION_CHANGED, ResolutionChanged);
    CHECK_NOTIFICATION_TYPE(CONFIG_CHANGED, ConfigChanged);
    CHECK_NOTIFICATION_TYPE(EXECUTE_SHORTCUT, ExecuteShortcut);
    CHECK_NOTIFICATION_TYPE(RELEASE_SHORTCUT, ReleaseShortcut);
    CHECK_NOTIFICATION_TYPE(EMULATION_STOPPED, EmulationStopped);
    CHECK_NOTIFICATION_TYPE(BEFORE_EMULATION_STOP, BeforeEmulationStop);
    CHECK_NOTIFICATION_TYPE(VIEWER_REFRESH, ViewerRefresh);
    CHECK_NOTIFICATION_TYPE(EVENT_VIEWER_REFRESH, EventViewerRefresh);
    CHECK_NOTIFICATION_TYPE(MISSING_FIRMWARE, MissingFirmware);
    CHECK_NOTIFICATION_TYPE(SUFAMI_TURBO_FILE_PROMPT, SufamiTurboFilePrompt);
    CHECK_NOTIFICATION_TYPE(BEFORE_GAME_UNLOAD, BeforeGameUnload);
    CHECK_NOTIFICATION_TYPE(BEFORE_GAME_LOAD, BeforeGameLoad);
    CHECK_NOTIFICATION_TYPE(GAME_LOAD_FAILED, GameLoadFailed);
    CHECK_NOTIFICATION_TYPE(CHEATS_CHANGED, CheatsChanged);
    CHECK_NOTIFICATION_TYPE(REQUEST_CONFIG_CHANGE, RequestConfigChange);
    CHECK_NOTIFICATION_TYPE(REFRESH_SOFTWARE_RENDERER, RefreshSoftwareRenderer);
    CHECK_NOTIFICATION_TYPE(START_AUDIO_DEVICE, StartAudioDevice);
    CHECK_NOTIFICATION_TYPE(STOP_AUDIO_DEVICE, StopAudioDevice);

    CHECK_VIDEO_FILTER_TYPE(NONE, None);
    CHECK_VIDEO_FILTER_TYPE(NTSC_BLARGG, NtscBlargg);
    CHECK_VIDEO_FILTER_TYPE(NTSC_BISQWIT, NtscBisqwit);
    CHECK_VIDEO_FILTER_TYPE(LCD_GRID, LcdGrid);
    CHECK_VIDEO_FILTER_TYPE(X_BRZ_2X, xBRZ2x);
    CHECK_VIDEO_FILTER_TYPE(X_BRZ_3X, xBRZ3x);
    CHECK_VIDEO_FILTER_TYPE(X_BRZ_4X, xBRZ4x);
    CHECK_VIDEO_FILTER_TYPE(X_BRZ_5X, xBRZ5x);
    CHECK_VIDEO_FILTER_TYPE(X_BRZ_6X, xBRZ6x);
    CHECK_VIDEO_FILTER_TYPE(HQ_2X, HQ2x);
    CHECK_VIDEO_FILTER_TYPE(HQ_3X, HQ3x);
    CHECK_VIDEO_FILTER_TYPE(HQ_4X, HQ4x);
    CHECK_VIDEO_FILTER_TYPE(SCALE_2X, Scale2x);
    CHECK_VIDEO_FILTER_TYPE(SCALE_3X, Scale3x);
    CHECK_VIDEO_FILTER_TYPE(SCALE_4X, Scale4x);

    CHECK_HUD_DISPLAY_SIZE(FIXED, Fixed);
    CHECK_HUD_DISPLAY_SIZE(SCALED, Scaled);

    CHECK_SHORTCUT(EXEC_RESET, ExecReset);
    CHECK_SHORTCUT(REWIND_10_SECONDS, RewindTenSecs);
    CHECK_SHORTCUT(REWIND, Rewind);

    CHECK_STRUCT(MesenExecuteShortcutParams, ExecuteShortcutParams);
    // CHECK_STRUCT(MesenSoftwareRendererSurface, SoftwareRendererSurface);
}