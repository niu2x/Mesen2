#include "VirtualKey.h"

VirtualKeyGroup nes_key_group = { .group_name = "Nes",
                                  .keys = { {
                                      APP_VK_A,
                                      APP_VK_B,
                                      APP_VK_TURBO_A,
                                      APP_VK_TURBO_B,
                                      APP_VK_UP,
                                      APP_VK_DOWN,
                                      APP_VK_LEFT,
                                      APP_VK_RIGHT,
                                      APP_VK_SELECT,
                                      APP_VK_START,
                                  } } };

static QKeySequence __default_keys[] = {
    Qt::Key_unknown,
    QKeySequence(Qt::Key_K),
    QKeySequence(Qt::Key_U),
    QKeySequence(Qt::Key_I),
    QKeySequence(Qt::Key_J),
    QKeySequence(Qt::Key_W),
    QKeySequence(Qt::Key_S),
    QKeySequence(Qt::Key_A),
    QKeySequence(Qt::Key_D),
    QKeySequence(Qt::Key_Space),
    QKeySequence(Qt::Key_Return),
    QKeySequence(Qt::Key_B),

};

QKeySequence* default_keys = __default_keys;

const char* vk_names[] = {
    "", "A", "B", "TURBO_A", "TURBO_B", "UP", "DOWN", "LEFT", "RIGHT", "SELECT", "START", "REWIND",
};