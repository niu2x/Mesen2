#include <Mesen/Mesen.h>
#include <chrono>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>

void notify(MesenNotificationType noti, void* param) { }

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Usage: %s <ROM_file> <run_duration>\n", argv[0]);
        return 1;
    }
    mesen_init();
    mesen_initialize_emu("./", true, false, false);

    MesenNesConfig NES_config = {
        .user_palette = &mesen_default_palette,
    };
    mesen_set_NES_config(&NES_config);

    MesenVideoConfig video_config {
        .video_filter = MESEN_VIDEO_FILTER_TYPE_NONE,
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

    std::this_thread::sleep_for(std::chrono::seconds { atoi(argv[2]) });

    mesen_release();
    return 0;
}