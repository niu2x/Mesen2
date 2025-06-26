#pragma once

#include <QAudioOutput>
#include <QIODevice>
#include <QtCore>
#include <cmath>

class GameSoundDevice : public QIODevice {
public:
    explicit GameSoundDevice(QObject* parent = nullptr);
    void start() { bool succ = open(QIODevice::ReadOnly); }
    void stop() { close(); }

protected:
    qint64 readData(char* data, qint64 maxlen) override;
    qint64 writeData(const char*, qint64) override { return 0; }
};
