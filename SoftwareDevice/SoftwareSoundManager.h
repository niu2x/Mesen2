#pragma once

#include "Core/Shared/Audio/BaseSoundManager.h"
#include <mutex>

class Emulator;

struct AudioDeviceParam {
    uint32_t bufferSize;
    uint32_t sampleRate;
    bool isStereo;
};

class SoftwareSoundManager : public BaseSoundManager {
public:
    SoftwareSoundManager(Emulator* emu);
    ~SoftwareSoundManager();

    void PlayBuffer(int16_t* soundBuffer,
                    uint32_t bufferSize,
                    uint32_t sampleRate,
                    bool isStereo) override;
    void Pause() override;
    void Stop() override;

    void ProcessEndOfFrame() override;

    string GetAvailableDevices() override;
    void SetAudioDevice(string deviceName) override;
    void FillAudioBuffer(uint8_t* stream, int len);

private:
    bool InitializeAudio(uint32_t sampleRate, bool isStereo);
    void Release();

    void ReadFromBuffer(uint8_t* output, uint32_t len);
    void WriteToBuffer(uint8_t* output, uint32_t len);

private:
    Emulator* _emu;
    string _deviceName;
    bool _needReset = false;
    uint16_t _previousLatency = 0;

    std::recursive_mutex buffer_mutex_;

    uint8_t* _buffer = nullptr;
    uint32_t write_position_ = 0;
    uint32_t read_position_ = 0;
    uint32_t data_count_ = 0;
    bool audio_playing_ = false;
};
