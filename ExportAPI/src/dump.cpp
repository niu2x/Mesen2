#include "dump.h"

const char* get_ROM_format_name(RomFormat ROM_format)
{
    switch (ROM_format) {
    case RomFormat::Unknown: {
        return "Unknown";
    }
    case RomFormat::Sfc: {
        return "SFC";
    }
    case RomFormat::Spc: {
        return "SPC";
    }
    case RomFormat::Gb: {
        return "GB";
    }
    case RomFormat::Gbs: {
        return "GBS";
    }
    case RomFormat::iNes: {
        return "iNES";
    }
    case RomFormat::Unif: {
        return "UNIF";
    }
    case RomFormat::Fds: {
        return "FDS";
    }
    case RomFormat::VsSystem: {
        return "VsSystem";
    }
    case RomFormat::VsDualSystem: {
        return "VsDualSystem";
    }
    case RomFormat::Nsf: {
        return "NSF";
    }
    case RomFormat::StudyBox: {
        return "StudyBox";
    }
    case RomFormat::Pce: {
        return "Pce";
    }
    case RomFormat::PceCdRom: {
        return "PceCdRom";
    }
    case RomFormat::PceHes: {
        return "PceHes";
    }
    case RomFormat::Sms: {
        return "SMS";
    }
    case RomFormat::GameGear: {
        return "GameGear";
    }
    case RomFormat::Sg: {
        return "Sg";
    }
    case RomFormat::ColecoVision: {
        return "ColecoVision";
    }
    case RomFormat::Gba: {
        return "GBA";
    }
    case RomFormat::Ws: {
        return "WS";
    }
    }
    return "";
}
const char* get_console_type_name(ConsoleType console_type)
{
    switch (console_type) {
    case ConsoleType::Snes: {
        return "sNES";
    }
    case ConsoleType::Gameboy: {
        return "Gameboy";
    }
    case ConsoleType::Nes: {
        return "NES";
    }
    case ConsoleType::PcEngine: {
        return "PcEngine";
    }
    case ConsoleType::Sms: {
        return "SMS";
    }
    case ConsoleType::Gba: {
        return "GBA";
    }
    case ConsoleType::Ws: {
        return "WS";
    }
    }
    return "";
}
const char* get_cpu_type_name(CpuType cpu_type)
{
    switch (cpu_type) {
    case CpuType::Snes: {
        return "sNES";
    }
    case CpuType::Gameboy: {
        return "Gameboy";
    }
    case CpuType::Nes: {
        return "NES";
    }
    case CpuType::Pce: {
        return "Pce";
    }
    case CpuType::Sms: {
        return "SMS";
    }
    case CpuType::Gba: {
        return "GBA";
    }
    case CpuType::Ws: {
        return "WS";
    }

    case CpuType::Spc: {
        return "SPC";
    }
    case CpuType::NecDsp: {
        return "NecDsp";
    }
    case CpuType::Sa1: {
        return "Sa1";
    }
    case CpuType::Gsu: {
        return "GSU";
    }
    case CpuType::Cx4: {
        return "Cx4";
    }
    case CpuType::St018: {
        return "St018";
    }
    }
    return "";
}
const char* get_notification_type_name(ConsoleNotificationType noti_type)
{
    switch (noti_type) {
    case ConsoleNotificationType::GameLoaded: {
        return "GameLoaded";
    }
    case ConsoleNotificationType::StateLoaded: {
        return "StateLoaded";
    }
    case ConsoleNotificationType::GameReset: {
        return "GameReset";
    }
    case ConsoleNotificationType::GamePaused: {
        return "GamePaused";
    }
    case ConsoleNotificationType::GameResumed: {
        return "GameResumed";
    }
    case ConsoleNotificationType::CodeBreak: {
        return "CodeBreak";
    }
    case ConsoleNotificationType::DebuggerResumed: {
        return "DebuggerResumed";
    }
    case ConsoleNotificationType::PpuFrameDone: {
        return "PpuFrameDone";
    }
    case ConsoleNotificationType::ResolutionChanged: {
        return "ResolutionChanged";
    }
    case ConsoleNotificationType::ConfigChanged: {
        return "ConfigChanged";
    }
    case ConsoleNotificationType::ExecuteShortcut: {
        return "ExecuteShortcut";
    }
    case ConsoleNotificationType::ReleaseShortcut: {
        return "ReleaseShortcut";
    }
    case ConsoleNotificationType::EmulationStopped: {
        return "EmulationStopped";
    }
    case ConsoleNotificationType::BeforeEmulationStop: {
        return "BeforeEmulationStop";
    }
    case ConsoleNotificationType::ViewerRefresh: {
        return "ViewerRefresh";
    }
    case ConsoleNotificationType::EventViewerRefresh: {
        return "EventViewerRefresh";
    }
    case ConsoleNotificationType::MissingFirmware: {
        return "MissingFirmware";
    }
    case ConsoleNotificationType::SufamiTurboFilePrompt: {
        return "SufamiTurboFilePrompt";
    }
    case ConsoleNotificationType::BeforeGameUnload: {
        return "BeforeGameUnload";
    }
    case ConsoleNotificationType::BeforeGameLoad: {
        return "BeforeGameLoad";
    }
    case ConsoleNotificationType::GameLoadFailed: {
        return "GameLoadFailed";
    }
    case ConsoleNotificationType::CheatsChanged: {
        return "CheatsChanged";
    }
    case ConsoleNotificationType::RequestConfigChange: {
        return "RequestConfigChange";
    }
    case ConsoleNotificationType::RefreshSoftwareRenderer: {
        return "RefreshSoftwareRenderer";
    }
    }
    return "";
}

std::ostream& operator<<(std::ostream& os, RomFormat ROM_format)
{
    os << get_ROM_format_name(ROM_format);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConsoleType console_type)
{
    os << get_console_type_name(console_type);
    return os;
}

std::ostream& operator<<(std::ostream& os, CpuType cpu_type)
{
    os << get_cpu_type_name(cpu_type);
    return os;
}
