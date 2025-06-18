#include <Mesen/Mesen.h>
#include <chrono>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <thread>

void notify(int noti, void* param)
{
    if (noti == MESEN_NOTIFICATION_TYPE_REFRESH_SOFTWARE_RENDERER) {
        auto* renderer_frame = (MesenSoftwareRendererFrame*)param;
        auto frame = renderer_frame->frame;
        auto emulator_HUD = renderer_frame->emulator_HUD;
        printf("frame %d %d\n", frame.width, frame.height);
        printf("emulator_HUD %d %d\n", emulator_HUD.width, emulator_HUD.height);
    }
}

int main(int argc, char* argv[])
{
    mesen_init();
    mesen_initialize_emu("./", true, false, false);

    MesenNesConfig NES_config = {
        .user_palette = {},
    };
    memcpy(NES_config.user_palette, mesen_default_palette, sizeof(MesenPalette));
    mesen_set_NES_config(&NES_config);

    MesenVideoConfig video_config {
        .video_filter = MESEN_VIDEO_FILTER_TYPE_HQ_4X,
        .brightness = 0,
    };
    mesen_set_video_config(&video_config);

    MesenPreferences preferences {
        .HUD_size = MESEN_HUD_DISPLAY_SIZE_SCALED,
        .show_fps = false,
        .show_frame_counter = false,
        .show_game_timer = false,
        .show_debug_info = false,
    };
    mesen_set_preferences(&preferences);

    mesen_register_notification_callback(notify);

    mesen_load_ROM(argv[1], NULL);

    std::this_thread::sleep_for(std::chrono::seconds { 1 });
    mesen_take_screenshot(argv[2]);

    mesen_release();
    return 0;
}