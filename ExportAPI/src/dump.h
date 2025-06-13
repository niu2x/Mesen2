#pragma once

#include <iostream>

#include "Core/Shared/CpuType.h"
#include "Core/Shared/Interfaces/INotificationListener.h"
#include "Core/Shared/RomInfo.h"
#include "Core/Shared/SettingTypes.h"

std::ostream& operator<<(std::ostream& os, RomFormat ROM_format);
std::ostream& operator<<(std::ostream& os, ConsoleType console_type);
std::ostream& operator<<(std::ostream& os, CpuType cpu_type);

const char* get_ROM_format_name(RomFormat ROM_format);
const char* get_console_type_name(ConsoleType ROM_format);
const char* get_cpu_type_name(CpuType ROM_format);
const char* get_notification_type_name(ConsoleNotificationType noti_type);