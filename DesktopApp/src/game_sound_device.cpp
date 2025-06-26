#include "game_sound_device.h"
#include <Mesen/Mesen.h>

GameSoundDevice::GameSoundDevice(QObject* parent)
    : QIODevice(parent) { }

qint64 GameSoundDevice::readData(char* data, qint64 maxlen) {
    // printf("maxlen %ld\n", maxlen);
    // maxlen = std::min(maxlen, (qint64) 1024);
    mesen_fill_audio_buffer(data, maxlen);
    return maxlen;
}