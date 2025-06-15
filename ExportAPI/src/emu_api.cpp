#include <Mesen/Mesen.h>
#include <InteropDLL/EmuApiWrapper.cpp>
#include "dump.h"

extern "C" {

uint32_t mesen_get_version() { return GetMesenVersion(); }

const char* mesen_get_build_date() { return GetMesenBuildDate(); }

void mesen_init() { InitDll(); }

void mesen_initialize_emu(const char* home_folder,
    bool no_audio,
    bool no_video,
    bool no_input)
{
    static struct {
    } fake_window;
    static struct {
    } fake_viewer;
    InitializeEmu(home_folder,
        &fake_window,
        &fake_viewer,
        true,
        no_audio,
        no_video,
        no_input);
}

void mesen_release() { Release(); }

bool mesen_load_ROM(const char* file, const char* patch_file)
{
    return LoadRom((char*)file, (char*)patch_file);
}

void mesen_dump_ROM_info()
{
    InteropRomInfo info;
    GetRomInfo(info);
    std::cout << "ROM Info: " << std::endl;
    std::cout << "    " << "ROM Format: " << info.Format << std::endl;
    std::cout << "    " << "Console: " << info.Console << std::endl;
    for (uint32_t i = 0; i < info.CpuTypeCount; i++)
        std::cout << "    " << "CpuType: " << info.CpuTypes[i] << std::endl;
}

bool mesen_is_running() { return IsRunning(); }
bool mesen_is_paused() { return IsPaused(); }

void mesen_take_screenshot() { TakeScreenshot(); }

double mesen_get_aspect_ratio() {
    return GetAspectRatio();
}

void* mesen_register_notification_callback(NotificationCallback callback)
{
    return RegisterNotificationCallback(callback);
}
void mesen_unregister_notification_callback(void* handle)
{
    UnregisterNotificationCallback((INotificationListener*)handle);
}

const char* mesen_get_notification_type_name(int noti_type)
{
    return get_notification_type_name((ConsoleNotificationType)noti_type);
}
}
