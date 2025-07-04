#include <Mesen/Mesen.h>
#include <InteropDLL/ConfigApiWrapper.cpp>

// #define RGBA_TO_ABGR(color) ( \
    // (((color) & 0xFF000000)) | (((color) & 0x00FF0000) >> 8) | (((color) & 0x0000FF00) << 0) | (((color) & 0x000000FF) << 16))

const MesenPalette mesen_default_palette = { 0xFF666666, 0xFF002A88, 0xFF1412A7, 0xFF3B00A4, 0xFF5C007E, 0xFF6E0040, 0xFF6C0600, 0xFF561D00, 0xFF333500, 0xFF0B4800, 0xFF005200, 0xFF004F08, 0xFF00404D, 0xFF000000, 0xFF000000, 0xFF000000, 0xFFADADAD, 0xFF155FD9, 0xFF4240FF, 0xFF7527FE, 0xFFA01ACC, 0xFFB71E7B, 0xFFB53120, 0xFF994E00, 0xFF6B6D00, 0xFF388700, 0xFF0C9300, 0xFF008F32, 0xFF007C8D, 0xFF000000, 0xFF000000, 0xFF000000, 0xFFFFFEFF, 0xFF64B0FF, 0xFF9290FF, 0xFFC676FF, 0xFFF36AFF, 0xFFFE6ECC, 0xFFFE8170, 0xFFEA9E22, 0xFFBCBE00, 0xFF88D800, 0xFF5CE430, 0xFF45E082, 0xFF48CDDE, 0xFF4F4F4F, 0xFF000000, 0xFF000000, 0xFFFFFEFF, 0xFFC0DFFF, 0xFFD3D2FF, 0xFFE8C8FF, 0xFFFBC2FF, 0xFFFEC4EA, 0xFFFECCC5, 0xFFF7D8A5, 0xFFE4E594, 0xFFCFEF96, 0xFFBDF4AB, 0xFFB3F3CC, 0xFFB5EBF2, 0xFFB8B8B8, 0xFF000000, 0xFF000000 };

void mesen_set_output_to_stdout(bool enable) {
    SetEmulationFlag(EmulationFlags::OutputToStdout, enable);
}

ControllerConfig from(const MesenControllerConfig& src)
{
    ControllerConfig config;
    config.Type = (ControllerType)src.type;
    config.Keys = {
        .Mapping1 = {
            .A = src.key_mapping.A,
            .B = src.key_mapping.B,
            .Up = src.key_mapping.up,
            .Down = src.key_mapping.down,
            .Left = src.key_mapping.left,
            .Right = src.key_mapping.right,
            .Start = src.key_mapping.start,
            .Select = src.key_mapping.select,
            .TurboA = src.key_mapping.turbo_A,
            .TurboB = src.key_mapping.turbo_B,
        },
        .TurboSpeed = src.key_mapping.turbo_speed,
    };
    return config;
}

void mesen_set_nes_config(const MesenNesConfig* NES_config) {
    auto config = GetNesConfig();
    memcpy(config.UserPalette, NES_config->user_palette, sizeof(MesenPalette));

    config.Port1 = from(NES_config->port_1);
    config.Port2 = from(NES_config->port_2);

    for (int i = 0; i < 11; i++)
        config.ChannelVolumes[i] = 100;

    SetNesConfig(config);
}

void mesen_set_snes_config(const MesenSnesConfig* config) {
    auto curr_config = _emu->GetSettings()->GetSnesConfig();

    curr_config.Port1 = from(config->port_1);
    curr_config.Port2 = from(config->port_2);

    for (int i = 0; i < 8; i++)
        curr_config.ChannelVolumes[i] = 100;

    SetSnesConfig(curr_config);
}

void mesen_set_video_config(const MesenVideoConfig* video_config)
{
    VideoConfig curr_config = _emu->GetSettings()->GetVideoConfig();

    curr_config.VideoFilter = (VideoFilterType)video_config->video_filter;
    curr_config.Brightness = video_config->brightness;
    curr_config.Contrast = video_config->contrast;
    curr_config.Hue = video_config->hue;
    curr_config.Saturation = video_config->saturation;
    curr_config.UseBilinearInterpolation = true;

    _emu->GetSettings()->SetVideoConfig(curr_config);
}

void mesen_set_preferences(const MesenPreferences* preferences)
{
    auto p = _emu->GetSettings()->GetPreferences();
    p.HudSize = (HudDisplaySize)preferences->HUD_size;
    p.ShowFps = preferences->show_fps;
    p.ShowFrameCounter = preferences->show_frame_counter;
    p.ShowGameTimer = preferences->show_game_timer;
    p.ShowDebugInfo = preferences->show_debug_info;
    p.DisableOsd = preferences->disable_on_screen_display;
    SetPreferences(p);
}

void mesen_set_shortcut_keys(MesenShortcutKeyInfo p_shortcuts[], int count) {
    static ShortcutKeyInfo shortcuts[512];
    for (int i = 0; i < count; i++) {
        shortcuts[i].Shortcut = (EmulatorShortcut)p_shortcuts[i].shortcut;
        shortcuts[i].Keys = {
            .Key1 = p_shortcuts[i].key_combination.keys[0],
            .Key2 = p_shortcuts[i].key_combination.keys[1],
            .Key3 = p_shortcuts[i].key_combination.keys[1],
        };
    }
    SetShortcutKeys(shortcuts, count);
}