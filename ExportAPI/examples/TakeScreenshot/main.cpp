#include <Mesen/Mesen.h>
#include <chrono>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <thread>

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

    mesen_load_ROM(argv[1], NULL);

    std::this_thread::sleep_for(std::chrono::seconds {8});

    mesen_take_screenshot(argv[2]);

    mesen_release();
    return 0;
}