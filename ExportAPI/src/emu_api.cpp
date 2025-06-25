#include <Mesen/Mesen.h>
#include <InteropDLL/EmuApiWrapper.cpp>
#include "dump.h"

uint32_t mesen_get_version() { return GetMesenVersion(); }

const char* mesen_get_build_date() { return GetMesenBuildDate(); }

void mesen_init() { InitDll(); }

void mesen_initialize_emu(const char* home_folder,
    bool no_audio,
    bool no_video,
    bool no_input)
{
    static struct {
        char _;
    } fake_window;
    static struct {
        char _;
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

bool mesen_is_running() { return IsRunning(); }
bool mesen_is_paused() { return IsPaused(); }

void mesen_take_screenshot(const char* save_to_path) { _emu->GetVideoDecoder()->TakeScreenshot(save_to_path); }

double mesen_get_aspect_ratio() {
    return GetAspectRatio();
}

MesenNotificationCallback user_notification_callback = NULL;
static void forward_notification_callback(int type, void* param) {
    user_notification_callback((MesenNotificationType)type, param);
}

void* mesen_register_notification_callback(MesenNotificationCallback callback) {
    user_notification_callback = callback;
    return RegisterNotificationCallback(forward_notification_callback);
}
void mesen_unregister_notification_callback(void* handle)
{
    UnregisterNotificationCallback((INotificationListener*)handle);
}

const char* mesen_get_notification_type_name(int noti_type)
{
    return get_notification_type_name((ConsoleNotificationType)noti_type);
}

void mesen_add_known_game_folder(const char* folder)
{
    AddKnownGameFolder((char*)folder);
}

void mesen_stop() { Stop(); }
void mesen_pause() { Pause(); }
void mesen_resume() { Resume(); }

void mesen_display_message(const char* title, const char* msg, const char* param1, const char* param2)
{
    MessageManager::DisplayMessage(title, msg, param1, param2);
}

void mesen_execute_shortcut(const MesenExecuteShortcutParams* exec_params)
{
    ExecuteShortcut({
        .Shortcut = (EmulatorShortcut)exec_params->shortcut,
        .Param = exec_params->param,
        .ParamPtr = exec_params->param_ptr,
    });
}

size_t mesen_get_log(char* out_buffer, size_t max_length) {
    auto logs = MessageManager::GetLog();
    StringUtilities::CopyToBuffer(logs, out_buffer, max_length);
    MessageManager::ClearLog();
    return logs.size();
}
