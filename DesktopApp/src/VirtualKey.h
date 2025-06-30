#pragma once

#include <QKeySequence>
#include <map>
#include <string>
#include <vector>

enum {
    APP_VK_BEGIN = 1,
    APP_VK_A = 1,
    APP_VK_B,
    APP_VK_TURBO_A,
    APP_VK_TURBO_B,
    APP_VK_UP,
    APP_VK_DOWN,
    APP_VK_LEFT,
    APP_VK_RIGHT,
    APP_VK_SELECT,
    APP_VK_START,
    APP_VK_REWIND,
    APP_VK_END,
};

extern const char* vk_names[];

struct VirtualKeyGroup {
    std::string group_name;
    std::vector<int> keys;
};

extern VirtualKeyGroup nes_key_group;
extern QKeySequence* default_keys;
